# Lexingine — Design Document

**Project:** Adventure Designer Studio (ADS)
**Component:** Lexingine — Lexicon Engine
**Version:** 0.1 (draft)
**Language:** C++23
**Status:** Design phase

---

## 1. What is Lexingine?

Lexingine is the language processing engine of Adventure Designer Studio. Its
responsibility is to bridge the gap between human-readable game text authored
on a PC and the compact binary token representation that runs on severely
memory-constrained 8-bit target platforms.

The name reflects its dual nature: it is both a **lexicon** — a structured
vocabulary of the game's words — and an **engine** — an active processing
pipeline that analyses, classifies, compresses, and dispatches language at
every stage of a game's lifecycle.

Lexingine operates in two distinct environments:

```
+---------------------------+        +---------------------------+
|   Authoring environment   |        |   Target runtime          |
|   (PC — the IDE)          |        |   (8-bit — ZX Spectrum,   |
|                           |        |    CPC, MSX, C64...)      |
|  Full NLP via UDPipe      |        |                           |
|  Incremental analysis     |  --->  |  Flat vocab_table in RAM  |
|  Frequency tracking       | compile|  Linear token lookup      |
|  Synonym detection        |        |  ParsedCommand dispatch   |
|  Binary emission          |        |  Affordance-driven action |
+---------------------------+        +---------------------------+
```

The fundamental principle: **all linguistic intelligence lives on the PC**.
By the time a game runs on an 8-bit target, every word has been classified,
compressed, and indexed. The target runtime does no NLP whatsoever — it only
reads pre-computed results.

---

## 2. Lexingine Parts

Lexingine is composed of six subsystems. Each has a clearly bounded
responsibility and a defined interface to the others.

---

### 2.1 Lexicon

The Lexicon is the central in-memory data structure that accumulates and
organises the game's vocabulary during authoring. It is built incrementally
as the creator writes text in the IDE.

The Lexicon holds one ordered collection per active language, sorted by
descending frequency. A secondary lookup index resolves any form — canonical
or synonym — to its entry in O(1).

```
Lexicon
├── ordered collection per language (sorted by descending frequency)
│     "es_ES" → [entry(freq=0.12), entry(freq=0.09), ...]
│     "en_US" → [entry(freq=0.15), entry(freq=0.11), ...]
│
└── lookup index
      "es_ES:coger"   → entry #3
      "es_ES:agarrar" → entry #3   ← synonym, resolves to same entry
      "en_US:take"    → entry #7
```

**Languages** follow BCP-47 with mandatory region variant: `es_ES`, `en_US`,
`de_DE`, `fr_FR`, `pt_BR`. The Lexicon treats `es_ES` and `es_MX` as
independent entries — separate frequency tables, separate token assignments.

**Entry IDs** are stable at design time. They are assigned on first insertion
and never change, regardless of frequency reordering. Token indices are
volatile and only assigned at compile time.

#### Lexicon entry node

Each node in the Lexicon represents one canonical word form within a specific
language. This is its persistent JSON representation:

```json
{
  "id":           "<uint32_t>",
  "lang":         "<string>",
  "canonical":    "<string>",
  "role":         "<uint8_t>",
  "types":        "<uint16_t>",
  "synonyms":     ["<uint32_t>", "..."],
  "frequency":    "<float>",
  "raw_count":    "<uint32_t>",
  "synonym_meta": [
    { "target": "<uint32_t>", "confidence": "<float>", "confirmed": "<bool>" }
  ]
}
```

| Field          | Type         | Description                                                     |
|----------------|--------------|-----------------------------------------------------------------|
| `id`           | `uint32_t`   | Stable design-time identifier. Never `0xFFFFFFFF` (reserved).   |
| `lang`         | `string`     | BCP-47 with region. Max 8 chars.                                |
| `canonical`    | `string`     | Normalised word form from UDPipe lemma. Max 32 chars.           |
| `role` (1)     | `uint8_t`    | Bitmask: `0x01`=Output, `0x02`=Input, `0x03`=Both.              |
| `types`        | `uint16_t`   | WordType bitmask — all grammatical categories observed.         |
| `synonyms`     | `uint32_t[]` | IDs of equivalent entries. Forward references allowed.          |
| `frequency`    | `float`      | Normalised ratio: `raw_count / total_corpus_tokens`.            |
| `raw_count`    | `uint32_t`   | Absolute occurrence count. Never decremented.                   |
| `synonym_meta` | array        | Confidence and confirmation state per synonym link.             |

> (1)  
> role — what the word is used for in the game
> A word in the Lexicon can serve two completely different purposes:
>  
> - **Output** — it appears in text that the game shows to the player. For example, the word "dark" in "You are in a dark room." The player reads it but never types it.
> - **Input** — it is a word the player can type and the parser will recognise. For example "take" or "key".
> - **Both** — it appears in output text AND the player can type it. Most nouns fall here: "key" appears in descriptions AND the player types "take key".
>   
> The role field is a bitmask because a word can belong to one or both categories independently:
>> 0x01 = 0b00000001 = Output only  
>> 0x02 = 0b00000010 = Input only  
>> 0x03 = 0b00000011 = Both (Output + Input)
> 
> In practice this matters at compile time. Words with the Input bit set go into vocab_table in RAM — the parser searches them at runtime. Words with only the Output bit skip vocab_table entirely, saving RAM. A word like "the" is Output only — the game text uses it but no sane player would type "take the the".

Fields not persisted to JSON (runtime or compile-time only): `dominant_type`, `type_counts`, `upos_raw`, `feats`, `stem`, `compiled_token`.

> Fields not persisted to JSON  
> These are fields that exist in the LexEntry C++ class in memory during authoring and compilation, but are never written to the JSON file on disk. The reason differs per field:  
> - **dominant_type** — this is derived from types and type_counts. It is the single WordType bit with the highest count. It can always be recalculated from the other two fields when the JSON is loaded back, so storing it would be redundant.
> - **type_counts**[11] — the per-bit occurrence counts that track how many times each WordType was observed. These can be recalculated by replaying the corpus, or stored separately in a companion file if replay is too expensive. Not part of the core entry contract.
> - **upos_raw** — the raw UPOS string that UDPipe returned ("VERB", "NOUN"...). This is diagnostic information — useful during development to verify UDPipe's output, but not needed once types is correctly populated.
> - **feats** — the UD morphological features string ("Tense=Pres|Person=3"). Transient NLP metadata used by the synonym pipeline. Not needed after the Lexicon is fully built.
> - **stem** — the Snowball stem, only populated when no UDPipe model is available as a fallback. When UDPipe is active, canonical (the lemma) supersedes it entirely. No point persisting it.
> - **compiled_token** — the TokenIndex assigned by the compiler. This changes every time the game is compiled, because frequencies may have shifted and the ordering may be different. It is always regenerated from scratch at compile time, so persisting it would give a false sense of stability.
> 
> In short: if a field can be derived from other persisted fields, or if it is only meaningful during a transient phase (NLP analysis or compilation), it does not belong in the persistent JSON representation.

#### WordType bitmask

`WordType` is a `uint16_t` bitmask where each bit represents one independent
grammatical category. A single entry may carry multiple bits simultaneously,
reflecting lexical ambiguity observed in the corpus.

| Bit   | Category     | UD UPOS source                     |
|-------|--------------|------------------------------------|
| 0     | Verb         | `VERB`, `AUX`                      |
| 1     | Noun         | `NOUN`, `PROPN`                    |
| 2     | Adjective    | `ADJ`                              |
| 3     | Adverb       | `ADV`                              |
| 4     | Preposition  | `ADP`                              |
| 5     | Article      | `DET`                              |
| 6     | Conjunction  | `CCONJ`, `SCONJ`                   |
| 7     | Pronoun      | `PRON`                             |
| 8     | Interjection | `INTJ`                             |
| 9     | Punctuation  | `PUNCT` — filtered, never inserted |
| 10    | Other        | `NUM`, `SYM`, `X`, `PART`          |
| 11–15 | —            | Spare for future use               |

When multiple bits are set, the **dominant type** is the category with the
highest observed occurrence count, recalculated automatically on every new
observation. The compiler emits a warning for ambiguous entries.

---

### 2.2 NLP Backend (UDPipe)

Lexingine delegates all linguistic analysis to **UDPipe 1.x**(2), a C++-native
NLP pipeline that performs tokenisation, lemmatisation, POS tagging, and
dependency parsing for over 50 languages.

UDPipe operates on **complete sentences**, not isolated words. This is
essential for correct disambiguation of lexically ambiguous forms — the
context of the full sentence is required for accurate POS tagging.

Lexingine never depends on `udpipe.h` directly. The UDPipe adapter translates
`udpipe::word` into an `NLPToken` — an internal plain data carrier — before
passing it to the Lexicon. This isolation keeps UDPipe swappable.

```
udpipe::word fields used:
  word.form      → NLPToken.form      (surface form, e.g. "corriendo")
  word.lemma     → NLPToken.lemma     (canonical form, e.g. "correr")
  word.upostag   → NLPToken.upos      (UD POS tag, e.g. "VERB")
  word.feats     → NLPToken.feats     (morphological features)
  word.head      → NLPToken.head      (dependency head index)
  word.deprel    → NLPToken.deprel    (dependency relation)

Fields not used: word.xpostag, word.misc
```

**Confidence convention:**

| Mode                       | Source                             | Confidence |
|----------------------------|------------------------------------|------------|
| Full UDPipe model loaded   | Lemmatiser + tagger active         | `1.0`      |
| Fallback (no model)        | Snowball stemmer + rule tokeniser  | `0.6`      |

In fallback mode the `stem` field is populated instead of `lemma`, and
`WordType` defaults to `Unknown` until the creator classifies the entry
manually.

> (2) Official Repository and related URLs
> - [Repository (master branch = UDPipe 1.x)](https://github.com/ufal/udpipe)  
> - [Official website and model downloads](https://ufal.mff.cuni.cz/udpipe/1)
> - [API reference](https://ufal.mff.cuni.cz/udpipe/1/api-reference)
> - [Releases page (latest stable is 1.3.0)](https://github.com/ufal/udpipe/releases)
> 
> For vcpkg integration, which is your dependency manager, UDPipe is available as a binary for 
> Linux, Windows, and OS X, and as a library for C++ GitHub, so it should integrate cleanly 
> into your CMakeLists. Worth checking if it is already in the vcpkg registry before adding it manually.

---

### 2.3 Synonym Pipeline

The synonym pipeline runs automatically on every new Lexicon insertion. It
operates in three layers applied in order, with short-circuit propagation: if
a layer produces a match, subsequent layers receive the reduced form rather
than the original.

```
Layer 1 — Affix trie (automatic, per-language)
    Detects productive prefixes and suffixes.
    Reduces "recoger" → root candidate "coger".
    Validates: root must already exist in Lexicon.
    If root does not exist → discards match (prevents "recabar" → "cabar").
    Confidence: 0.9 (affix + root exists), 0.7 (affix only).

Layer 2 — Snowball stemmer (fallback only, no UDPipe model)
    Groups entries sharing a common stem.
    Receives reduced root from Layer 1, or original form.
    Confidence: 0.6

Layer 3 — Semantic similarity (manual review)
    Proposes candidates based on semantic distance.
    All proposals require explicit creator confirmation.
    Confidence: 0.4 before confirmation.
```

**Confidence levels:**

| Origin                          | `confidence` | `confirmed` |
|---------------------------------|--------------|-------------|
| Creator-defined (manual)        | `1.0`        | `true`      |
| Layer 1: affix + root exists    | `0.9`        | `false`     |
| Layer 1: affix only             | `0.7`        | `false`     |
| Layer 2: shared stem (fallback) | `0.6`        | `false`     |
| Layer 3: semantic candidate     | `0.4`        | `false`     |

The compiler can be configured to emit only synonyms where `confirmed = true`
or `confidence >= threshold`.

---

### 2.4 Token Encoder

The Token Encoder is responsible for assigning `TokenIndex` values at compile
time and serialising the Lexicon into the binary structures that live in the
target's RAM.

#### TokenIndex encoding

The `TokenIndex` (`uint16_t`) is serialised into `message_pool` with variable
length, using `0xFF` as an escape byte:

| ID Range            | Bytes in RAM | Format                                |
|---------------------|--------------|---------------------------------------|
| `0x0000`            | 1 byte       | `[00]` — end of message (EOL)         |
| `0x0001` – `0x00FE` | **1 byte**   | `[id]` — top-254 most frequent tokens |
| `0x00FF`            | —            | reserved escape byte                  |
| `0x0100` – `0xFFFE` | **3 bytes**  | `[FF][hi][lo]` — remaining tokens     |
| `0xFFFF`            | —            | UNASSIGNED sentinel                   |

The decoder on the target reads one byte. If it is not `0xFF`, that byte is
the token index directly. If it is `0xFF`, the following two bytes form the
extended index. The protocol is self-describing — no state required between
tokens.

```asm
Decoder pseudocode (Z80):

  decode_token:
      LD A, (HL)        ; read byte
      INC HL
      CP 0xFF           ; escape?
      JR Z, .extended
      RET               ; A = token index (1 byte)
  .extended:
      LD B, (HL)        ; high byte
      INC HL
      LD C, (HL)        ; low byte
      INC HL
      RET               ; BC = token index (2 bytes)
```

#### Indexing algorithm

```
1. Sort all Lexicon entries by descending frequency per language.
2. Assign TokenIndex 0x0001..0x00FE to the top-254 entries (1-byte tokens).
3. Assign TokenIndex 0x0100..0xFFFE to all remaining entries (3-byte tokens).
4. Emit vocab_table: array of (string pointer, TokenIndex, WordType) per entry.
5. Serialise message_pool: each game message as a token ID array + 0x0000 EOL.
```

#### Inflected forms (morphologically rich languages)

For languages with rich morphology (Russian, German), the compiler inserts
multiple surface forms per token ID. All forms map to the same `TokenIndex`.

The number of forms N per word is calculated automatically:

```
N = clamp(
      floor(ROM_available / (vocab_size × bytes_per_form)),
      N_min = 1,
      N_max = forms_available_from_UDPipe
    )
```

The compiler emits a warning if `N = 1` for any language where the player is
likely to write inflected forms.

---

### 2.5 Target Runtime Parser

The parser runs on the 8-bit target machine. It reads player input, identifies
meaningful tokens by looking them up in `vocab_table`, and produces a
`ParsedCommand` that the affordance dispatch engine uses to trigger game
actions.

**The parser performs no linguistic analysis.** All classification was done at
compile time. The parser only reads `WordType` values that are already baked
into `vocab_table`.

#### ParsedCommand

```c++
ParsedCommand {
    verb  : uint16_t    -- always present; 0x0000 if missing
    noun1 : uint16_t    -- primary object; 0x0000 if absent
    prep  : uint16_t    -- preposition token; 0x0000 if absent
    noun2 : uint16_t    -- secondary object; 0x0000 if absent
}
```

Slots set to `0x0000` act as wildcards in affordance matching. A condact that
does not specify `prep` or `noun2` matches any value in those slots.

**Direction words** (`NORTH`, `SOUTH`, `EAST`, `WEST`, `UP`, `DOWN`) are not
vocabulary tokens — they are direction constants resolved directly against the
current Room's exits. `GO NORTH` dispatches to the Room object with direction
constant `NORTH`, requiring only one lookup instead of two.

```c++
ParsedCommand for "GO NORTH":
    verb  = GO      (engine verb)
    noun1 = NORTH   (direction constant — high byte flags as direction)
    prep  = 0x0000
    noun2 = 0x0000
```

#### Sentence patterns

**Pattern A — V + N + PREP + N**

```
"put key in box"
    verb=PUT  noun1=KEY  prep=IN  noun2=BOX
```

**Pattern B — V + N + CONJ + V + N**

Produces two sequential `ParsedCommand` records executed in order:

```
"take key and open box"
    cmd[0]: verb=TAKE  noun1=KEY  prep=0  noun2=0
    cmd[1]: verb=OPEN  noun1=BOX  prep=0  noun2=0
```

#### Tokenisation loop slot rules

- `verb_slot` — overwritten by the most recently seen verb.
- `noun1_slot` — fills first available noun. Once a PREP or CONJ is seen,
  subsequent nouns go to `noun2_slot`.
- `prep / conj flag` — PREP tokens pass through to `ParsedCommand.prep`.
  CONJ tokens flush current slots into `cmd[0]` and restart collection
  for `cmd[1]`.

#### Lexical ambiguity tiebreaking

When a word carries multiple `WordType` bits, the parser applies a positional
rule at zero extra cost:

```
IF no verb found yet AND word has Verb bit set
THEN treat as verb
ELSE IF word has Noun bit set
THEN treat as noun
ELSE use dominant_type from vocab_table entry
```

In Z80 assembly this is a single `BIT 0, (WordType)` test.

---

### 2.6 Affordance Dispatch

Affordance dispatch is the bridge between the parser's output and the game's
response. It connects `ParsedCommand` to the object that should react, and
determines which behaviour to execute.

This subsystem is covered in detail in the ADS Object Affordance System
design document. The Lexingine interface to it is:

```
dispatch(ParsedCommand, game_world)
    │
    ├── verb is engine verb (GO, INVENTORY, SAVE...)
    │       → execute engine behaviour directly
    │
    └── verb is affordance verb (TAKE, EAT, OPEN...)
            │
            ├── find target object via noun1 token
            │
            ├── check object has matching affordance
            │       NO  → emit "You can't do that."
            │       YES → execute default affordance behaviour
            │
            └── check object has trigger for this verb
                    NO  → done
                    YES → queue trigger with delay parameter
```

**Trigger delay** is expressed in game-defined time units (seconds, turns, or
custom units). The engine maintains a pending event queue checked on every
game turn.

---

## 3. Algorithms

### 3.1 Lexicon feed — word insertion and frequency update

```
  +-----------------------------+
  |   Receive NLPToken          |
  |   (lemma, upos, lang, ...)  |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   Is token lexical?         |
  |   (not PUNCT, not empty)    |
  +-----------------------------+
         |           |
        yes          no
         |           |
         v           v
  +----------+  +---------+
  |  Look up |  | Discard |
  |  in      |  +---------+
  |  Lexicon |       |
  |  by      |       |
  |  lemma   |       |
  |  + lang  |       |
  +----------+       |
         |           |
      found       not found
         |           |
         v           v
  +----------+  +--------------------+
  | observe  |  | Create new entry   |
  | type()   |  | canonical = lemma  |
  | accumu-  |  | type from upos     |
  | late()   |  | observe_type()     |
  +----------+  | accumulate()       |
         |      | run synonym pipe-  |
         |      | line               |
         |      +--------------------+
         |           |
         +-----------+
               |
               v
  +-----------------------------+
  |   Recalculate frequencies   |
  |   for all entries           |
  |   (raw_count / total)       |
  +-----------------------------+
```

---

### 3.2 Compile-time indexing

```
  +-----------------------------+
  |   For each active language  |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   Sort entries by           |
  |   descending frequency      |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   Assign TokenIndex         |
  |   0x0001..0x00FE            |
  |   to top-254 entries        |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   Assign TokenIndex         |
  |   0x0100..0xFFFE            |
  |   to remaining entries      |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   Calculate N inflected     |
  |   forms per word            |
  |   based on RAM budget       |
  +-----------------------------+
               |
               v
  +-----------------------------+       +-------------------+
  |   Emit vocab_table          | ----> |   Target RAM      |
  |   Emit message_pool         |       |   vocab_table[]   |
  |   Emit Huffman tree         |       |   message_pool[]  |
  |   (if profile uses it)      |       +-------------------+
  +-----------------------------+
```

---

### 3.3 Runtime parser — main flow

```
  +-----------------------------+
  |   Player presses Enter      |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   Read input buffer         |
  |   lowercase + normalise     |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   Tokenise words            |
  |   look up in vocab_table    |
  |   assign to slots           |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   CONJ detected?            |
  +-----------------------------+
         |           |
        yes          no
         |           |
         v           v
  +-----------+ +-----------+
  | Pattern B | | Pattern A |
  | cmd[0]    | | single    |
  | + cmd[1]  | | command   |
  +-----------+ +-----------+
         |           |
         +-----------+
               |
               v
  +-----------------------------+
  |   Valid V + N found?        |
  +-----------------------------+
         |           |
        yes          no
         |           |
         v           v
  +-----------+ +--------------------+
  | Build     | | Emit "I don't      |
  | Parsed-   | | understand"        |
  | Command[] | +--------------------+
  +-----------+
         |
         v
  +-----------------------------+
  |   Affordance dispatch       |
  +-----------------------------+
         |
         v
  +-----------------------------+
  |   Display response          |
  +-----------------------------+
```

---

### 3.4 Runtime parser — tokenisation loop

```
  +-----------------------------+
  |   Read next word            |
  +-----------------------------+
         |           |
     more words   end of input
         |           |
         v           v
  +-----------+ +-------+
  | Look up   | | Exit  |
  | vocab_    | +-------+
  | table     |      |
  +-----------+      |
         |           |
      found       not found
         |           |
         v           v
  +-----------+ +-----------+
  | Check     | | Ignore    |
  | WordType  | | word      |
  +-----------+ +-----------+
    |   |   |         |
   Verb Noun Prep/    |
    |   |   Conj      |
    v   v    v        |
  +---+---+------+   |
  |v  |n  |flag  |   |
  |slt|slt|<-tok |   |
  +---+---+------+   |
    |   |   |        |
    +---+---+--------+
               |
               v (loop — next word)
```

---

### 3.5 Affordance dispatch

```
  +-----------------------------+
  |   Receive ParsedCommand     |
  +-----------------------------+
               |
               v
  +-----------------------------+
  |   Is verb an engine verb?   |
  |   (GO, INVENTORY, SAVE...)  |
  +-----------------------------+
         |           |
        yes          no
         |           |
         v           v
  +-----------+ +--------------------+
  | Execute   | | Find target object |
  | engine    | | via noun1 token    |
  | behaviour | +--------------------+
  +-----------+           |
                          v
               +--------------------+
               | Object has         |
               | matching affordance|
               +--------------------+
                    |          |
                   yes         no
                    |          |
                    v          v
               +--------+ +------------------+
               | Execute| | Emit "You can't  |
               | default| | do that."        |
               | behav. | +------------------+
               +--------+
                    |
                    v
               +--------------------+
               | Object has trigger |
               | for this verb?     |
               +--------------------+
                    |          |
                   yes         no
                    |          |
                    v          v
               +--------+ +-------+
               | Queue  | | Done  |
               | trigger| +-------+
               | with   |
               | delay  |
               +--------+
```

---

## 4. Relations with Other Entities

### 4.1 Lexingine ↔ Item (object)

Items are the primary consumers of the affordance dispatch subsystem. Each
Item carries:

- An **affordance bitmask** that determines which verbs are valid for it.
- A **trigger table** that maps verbs to deferred custom behaviours.
- A **state bitmask** that the default affordance behaviours read and write.

Lexingine's relationship to Items is read-only at runtime: the parser produces
a `ParsedCommand`, dispatch finds the Item via `noun1`, and the Item's own
data drives what happens next. Lexingine never mutates Item state directly —
it calls the Item's affordance handler which does.

```
Lexingine                        Item
──────────────────────────────────────────────────────
ParsedCommand { verb=TAKE,  ──>  affordances: Takeable
                noun1=KEY }      state: NotCarried
                                 triggers:
                                   on_take: (none)

                            <──  default: move to inventory
                                 trigger: not fired
```

### 4.2 Lexingine ↔ Room

Rooms are objects with a `Navigable` affordance. They differ from Items in
one key way: movement verbs (`GO`, `TRAVEL`) dispatch to the **current Room**,
not to a named object in `noun1`.

Direction words (`NORTH`, `SOUTH`, `EAST`, `WEST`, `UP`, `DOWN`) are direction
constants, not vocabulary tokens. They are resolved directly against the
Room's exit table, requiring only one lookup at runtime.

```
Lexingine                        Room
──────────────────────────────────────────────────────
ParsedCommand { verb=GO,      ──>  affordances: Navigable
                noun1=NORTH}       exits:
                                     NORTH → Room #12
                                     SOUTH → (none)
  
                              <──  exit exists: move player to Room #12
                                   exit absent: "You can't go that way."
```

### 4.3 Lexingine ↔ Compiler (PC side)

On the PC, the compiler drives Lexingine through four sequential phases:

```
Compiler
    │
    ├── FEED phase
    │     Reads game JSON → passes text to LexiconBuilder
    │     LexiconBuilder calls UDPipe → receives NLPToken stream
    │     Lexicon grows incrementally
    │
    ├── ANALYSIS phase
    │     Synonym pipeline runs on completed Lexicon
    │     Phrase detection promotes frequent n-grams
    │
    ├── INDEXING phase
    │     Token Encoder sorts by frequency
    │     Assigns TokenIndex values
    │     Calculates N inflected forms per language
    │
    └── EMISSION phase
          Token Encoder serialises:
            vocab_table[]   → parser lookup table in RAM
            message_pool[]  → tokenised game texts in RAM
```

### 4.4 Lexingine ↔ i18n system

Lexingine uses the ADS i18n system's `LanguageCode` format directly (`es_ES`,
`en_US`, `de_DE`). Each language maps to an independent Lexicon collection,
an independent `vocab_table` in RAM, and an independent UDPipe model file.

The i18n system is responsible for selecting the active language at runtime.
Lexingine is responsible for ensuring the correct `vocab_table` is loaded for
that language.

---

## 5. Conclusions

Lexingine achieves its goals through a strict separation of concerns between
the authoring environment and the 8-bit runtime.

**All intelligence is compile-time.** UDPipe, the synonym pipeline, frequency
analysis, inflected form generation, and token index assignment all happen on
the PC. The 8-bit target receives only flat, pre-computed tables.

**The runtime footprint is minimal.** The parser requires approximately 600
bytes of Z80 code, a linear scan over ~200 `vocab_table` entries, and a
four-slot `ParsedCommand` record. This leaves the majority of available RAM
for game content.

**The affordance model is the correct abstraction.** Connecting verbs to
objects through affordances rather than a global condact table keeps game
logic local, scales naturally with object complexity, and does not require the
creator to understand parser internals.

**The design is platform-agnostic by construction.** The compiler emits
different binary profiles for each target (ZX Spectrum, CPC, MSX, C64, Atari
ST), varying token encoding strategy and inflected form count according to
available RAM. The authoring experience is identical regardless of target.
