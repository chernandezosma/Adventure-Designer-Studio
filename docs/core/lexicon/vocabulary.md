
<!--
Copyright (C) 2025 [Cayetano H. Osma](cayetano.hernandez.osma@gmail.com)

This file is part of this project.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
-->

# `Lexicon` System Design

## Context

The `Lexicon` is the compiler component of Adventure Designer Studio responsible for managing game vocabulary, calculating word frequencies, and producing the tokenized binary for 8-bit target platforms. It operates entirely in memory during compilation — there is no database or own persistence layer. Input is JSON; output is binary.

The system is built incrementally: the `Lexicon` grows and updates as the author writes text in the IDE, not in a single batch pass at compile time.

## Core Principles

- **All game text is stored as arrays of token IDs**, never as ASCII strings in ROM. A 38-character description can be reduced to 8–9 bytes.
- **The lexicon covers both output text and parser input** within the same structure, differentiated by the `role` field.
- **Entry IDs are stable at design time** — they are assigned on first insertion and never change regardless of frequency reordering. Token indices are volatile and only assigned at compile time.
- **First-class multilanguage support** — language is a primary attribute of each entry, not an external namespace. Format follows BCP-47 with region: `es_ES`, `en_US`, `de_DE`.
- **Lexical ambiguity is a first-class concern** — a single entry can carry multiple grammatical categories simultaneously, each with independent frequency tracking.

## NLP Backend: UDPipe

### Role

The `Lexicon` does not perform linguistic analysis itself. All tokenisation, lemmatisation, and part-of-speech tagging is delegated to **UDPipe 1.x**, a C++-native NLP pipeline developed by the Institute of Formal and Applied Linguistics at Charles University, Prague.

UDPipe processes complete sentences — not isolated words — which is essential for correct disambiguation of lexically ambiguous forms (e.g. "abierto" as verb participle vs. adjective). The context of the full sentence is required for accurate POS tagging.

### What UDPipe provides

For each word in a sentence, UDPipe returns:

| Field | Description | Example |
|---|---|---|
| `form` | Surface form as written | `"corriendo"` |
| `lemma` | Canonical dictionary form | `"correr"` |
| `upostag` | Universal Dependencies POS tag | `"VERB"` |
| `feats` | Morphological features | `"Tense=Pres\|Person=3"` |
| `head` | Dependency head index | `2` |
| `deprel` | Dependency relation | `"nsubj"` |

The `lemma` field is the primary key used by the `Lexicon` for lookup and insertion. This replaces any manual normalisation (lowercase, diacritic stripping) that would otherwise be needed — UDPipe's linguistic knowledge produces more accurate canonical forms.

### Adapter layer

The `Lexicon` never depends on `udpipe.h` directly. The UDPipe adapter translates `udpipe::word` into an `NLPToken` — an internal data carrier — before passing it to the `Lexicon`. This isolation keeps UDPipe swappable without touching the `Lexicon` code.

### Language model availability

UDPipe requires a trained model file per language (e.g. `spanish-gsd-ud-2.5.udpipe`). Models are available for all target languages: Spanish, English, German, French, Portuguese.

When no model is available for a given language, the system degrades gracefully to a rule-based fallback:

| Mode | `lemma` source | POS source | Confidence |
|---|---|---|---|
| Full UDPipe model | UDPipe lemmatiser | UDPipe tagger | `1.0` |
| Fallback (no model) | Snowball stemmer (approximate) | Not available | `0.6` |

In fallback mode, `stem` is populated instead of `lemma`, and `WordType` defaults to `Unknown` until the author classifies the entry manually.

### Supported languages (UDPipe models)

| Language | Code | Model availability |
|---|---|---|
| Spanish (Spain) | `es_ES` | Available |
| Spanish (Latin America) | `es_MX` | Available (shared model) |
| English (US) | `en_US` | Available |
| English (UK) | `en_GB` | Available (shared model) |
| German | `de_DE` | Available |
| French | `fr_FR` | Available |
| Portuguese (Portugal) | `pt_PT` | Available |
| Portuguese (Brazil) | `pt_BR` | Available (shared model) |

## Universal Dependencies POS Tags → WordType

UDPipe returns POS tags following the Universal Dependencies standard. These are mapped to the ADS `WordType` bitmask:

| UD UPOS | WordType | Notes |
|---|---|---|
| `VERB` | Verb | |
| `AUX` | Verb | Auxiliary verbs treated as verbs |
| `NOUN` | Noun | |
| `PROPN` | Noun | Proper nouns treated as nouns |
| `ADJ` | Adjective | |
| `ADV` | Adverb | |
| `ADP` | Preposition | |
| `DET` | Article | |
| `CCONJ` | Conjunction | Coordinating |
| `SCONJ` | Conjunction | Subordinating |
| `PRON` | Pronoun | |
| `INTJ` | Interjection | |
| `PUNCT` | Punctuation | Filtered — never inserted into the Lexicon |
| `NUM`, `SYM`, `X`, `PART` | Other | Kept, low priority |

## WordType: Bitmask Design

`WordType` is a `uint16_t` bitmask where each bit represents one independent grammatical category. A single entry may carry multiple bits simultaneously, reflecting genuine lexical ambiguity observed in the corpus.

### Bit layout

| Bit | Category | Example |
|---|---|---|
| 0 | Verb | `coger`, `abrir` |
| 1 | Noun | `espada`, `habitación` |
| 2 | Adjective | `oscuro`, `abierto` |
| 3 | Adverb | `rápidamente` |
| 4 | Preposition | `en`, `sobre` |
| 5 | Article | `el`, `una` |
| 6 | Conjunction | `y`, `pero` |
| 7 | Pronoun | `él`, `me` |
| 8 | Interjection | `oh`, `ay` |
| 9 | Punctuation | `.`, `,` — filtered |
| 10 | Other | numbers, symbols |
| 11–15 | — | spare for future use |

### Lexical ambiguity

When UDPipe observes a lemma with more than one POS category across the corpus, the corresponding bits are both set. The entry is flagged as ambiguous.

Each bit position maintains an independent occurrence count. The **dominant type** — the category with the highest count — is recalculated automatically on every new observation. The compiler emits a warning for every ambiguous entry, giving the author the option to split it into two distinct entries (one per WordType) at the cost of an additional token in ROM.

**Example — "abierto" (Spanish):**

```
"La puerta está abierta"  →  UDPipe: lemma="abrir", UPOS="ADJ"   → bit 2 set, count[2]++
"Abre la puerta"          →  UDPipe: lemma="abrir", UPOS="VERB"  → bit 0 set, count[0]++
```

After three ADJ observations and one VERB observation:
- `types` = `Verb | Adjective`
- `dominant_type` = `Adjective`
- Compiler warning issued

## Lexicon Entry Structure

Each entry represents one canonical word form within a specific language.

| Field | Type | Description |
|---|---|---|
| `id` | `uint32_t` | Stable design-time identifier. Assigned on insertion, never changes. |
| `lang` | `string` | BCP-47 language code with region (`es_ES`, `en_US`). |
| `canonical` | `string` | Canonical form from UDPipe lemma. Max 32 characters. |
| `role` | `EntryRole` | Output, Input, or Both (see below). |
| `types` | `uint16_t` | Bitmask of all observed WordType categories. |
| `dominant_type` | `uint16_t` | Single-bit WordType with highest occurrence count. |
| `type_counts` | `uint32_t[11]` | Per-bit occurrence count, indexed by bit position. |
| `upos_raw` | `string` | Raw UPOS string from UDPipe. Kept for debugging. |
| `feats` | `string` | UD morphological features string. |
| `stem` | `string` | Snowball stem. Populated only in fallback mode (no UDPipe model). |
| `synonyms` | `SynonymLink[]` | Directed synonym relationships (see Synonym Pipeline). |
| `frequency` | `float` | Normalised occurrence ratio: `raw_count / total_corpus_tokens`. |
| `raw_count` | `uint32_t` | Absolute occurrence count. Used for O(1) frequency recalculation. |
| `compiled_token` | `uint16_t` | Assigned by the compiler at index time. Invalid at design time. |

### Entry Role

| Value | Meaning |
|---|---|
| `Output` | Appears in text shown to the player |
| `Input` | Recognised by the parser (player can type it) |
| `Both` | Output and Input (default) |

## Canonical Node Structure

This is the authoritative definition of a `Lexicon` entry node as it exists in memory and as it is persisted to JSON. All fields listed here are present in every entry — optional fields default to the values shown.

```json
{
  "id":           "<uint32_t>",
  "lang":         "<string>",
  "canonical":    "<string>",
  "role":         "<uint8_t>",
  "types":        "<uint16_t>",
  "synonyms":     ["<uint32_t>", "..."],
  "frequency":    "<float>",
  "raw_count":    "<uint32_t>"
}
```

### Field reference

| Field | Type | Constraints | Description |
|---|---|---|---|
| `id` | `uint32_t` | Assigned on insertion. Never `0xFFFFFFFF` (reserved as INVALID). | Stable design-time identifier. Never changes across sessions or recompilations. |
| `lang` | `string` | BCP-47 with region. Max 8 chars. e.g. `"es_ES"`, `"en_US"`. | Language of this entry. Entries with the same `canonical` but different `lang` are independent entries. |
| `canonical` | `string` | Max 32 chars. Source: UDPipe lemma. Fallback: lowercase + diacritics stripped. | The single normalised word form that represents this entry. All synonyms and inflected forms resolve to this. |
| `role` | `uint8_t` | Bitmask. `0x01` = Output, `0x02` = Input, `0x03` = Both. Default: `0x03`. | Whether this entry participates in output text, parser input, or both. |
| `types` | `uint16_t` | Bitmask. One bit per grammatical category (see WordType bit layout). Default: `0x0000`. | All grammatical categories observed for this lemma across the corpus. Set automatically by the NLP pipeline; may be adjusted by the author. |
| `synonyms` | `uint32_t[]` | Each element is a valid `id` of another entry in the same language. May be empty. Forward references (target not yet inserted) are resolved at index time. | List of entry IDs that are semantically equivalent to this entry. The relationship is directional: this entry is canonical; the listed entries are variants that resolve to it. |
| `frequency` | `float` | Range `[0.0, 1.0]`. Derived: `raw_count / total_corpus_tokens`. | Normalised occurrence frequency. Recalculated on every new observation. Used for sorting and token index assignment. |
| `raw_count` | `uint32_t` | Monotonically increasing. Never decremented. | Absolute occurrence count across all text fed to the Lexicon. Allows O(1) frequency recalculation without replaying the corpus. |

### Fields not persisted to JSON

The following fields are runtime or compile-time concerns and are never written to the JSON representation:

| Field | Reason |
|---|---|
| `dominant_type` | Derived from `types` + `type_counts`. Recalculated on load. |
| `type_counts[11]` | Per-observation history. Recalculated on load or stored in a companion file. |
| `upos_raw` | Diagnostic only. Not part of the persistent contract. |
| `feats` | Transient NLP metadata. Not required after lemma and types are established. |
| `stem` | Fallback only. Not persisted when a UDPipe model is active. |
| `compiled_token` | Compile-time assignment. Regenerated on every compilation. |

### Synonym metadata

Each element in `synonyms` carries an `id` reference only. Confidence and confirmation state are stored in a companion `synonym_meta` block within the same entry:

```json
{
  "id": 3,
  "lang": "es_ES",
  "canonical": "coger",
  "role": 3,
  "types": 2,
  "synonyms": [7, 12, 45],
  "frequency": 0.12,
  "raw_count": 84,
  "synonym_meta": [
    { "target": 7,  "confidence": 1.0, "confirmed": true  },
    { "target": 12, "confidence": 0.9, "confirmed": false },
    { "target": 45, "confidence": 0.4, "confirmed": false }
  ]
}
```

`synonym_meta` is optional. If absent, all entries in `synonyms` are treated as manually confirmed (`confidence: 1.0, confirmed: true`).

### Complete example

```json
{
  "id": 3,
  "lang": "es_ES",
  "canonical": "abrir",
  "role": 3,
  "types": 5,
  "synonyms": [18, 42],
  "frequency": 0.09,
  "raw_count": 63,
  "synonym_meta": [
    { "target": 18, "confidence": 0.9, "confirmed": false },
    { "target": 42, "confidence": 1.0, "confirmed": true  }
  ]
}
```

Interpretation:

- `role: 3` → `0x03` = Both (Output + Input)
- `types: 5` → `0b00000101` = bits 0 and 2 = `Verb | Adjective` — lexically ambiguous
- `synonyms: [18, 42]` → entry 18 is an unconfirmed affix match (confidence 0.9); entry 42 was manually confirmed by the author

## Token ID Encoding

The `compiled_token` (`uint16_t`) is serialised into `message_pool` with variable length:

| ID Range | Bytes in ROM | Format | Usage |
|---|---|---|---|
| `0x0000` | 1 byte | `[00]` | End of message (EOL) |
| `0x0001` – `0x00FE` | **1 byte** | `[id]` | Top-254 most frequent tokens |
| `0x00FF` | — | reserved | Escape byte — never assigned as ID |
| `0x0100` – `0xFFFE` | **3 bytes** | `[FF][hi][lo]` | All remaining tokens |
| `0xFFFF` | — | sentinel | UNASSIGNED — never emitted |

The decoder on the 8-bit target reads one byte. If it is not `0xFF`, that byte is the token index directly. If it is `0xFF`, the next two bytes form the extended index. The protocol is self-describing — no state is required between tokens.

## In-Memory Organisation (Compiler, C++)

The `Lexicon` holds one ordered collection per active language, sorted by descending frequency. A secondary lookup index resolves any form — canonical or synonym — to its entry in O(1).

```
Lexicon
├── ordered collection per language (sorted by descending frequency)
│     "es_ES" → [entry(freq=0.12), entry(freq=0.09), ...]
│     "en_US" → [entry(freq=0.15), entry(freq=0.11), ...]
│
└── lookup index (form+lang → entry)
      "es_ES:coger"   → entry #3
      "es_ES:agarrar" → entry #3   ← synonym resolves to same entry
      "es_ES:tomar"   → entry #3   ← synonym resolves to same entry
      "en_US:take"    → entry #7
```

## ROM Table Separation on the Target

Although the compiler uses a single `Lexicon`, two independent structures are generated in the 8-bit target ROM:

```
Target ROM
│
├── vocab_table[]
│     Array of pointers to canonical ASCII strings.
│     Indexed directly by token ID — O(1) lookup.
│     Searched linearly when parsing player input (~200 entries, ~2ms on Z80).
│
└── message_pool[]
      Arrays of token IDs representing game texts.
      Traversed sequentially for output only.
      Never searched — direct index into vocab_table.
```

Increasing the number of unique words only affects the size of `vocab_table` in ROM — it does not affect the speed of any runtime operation.

## Synonym Pipeline

Synonym relationships are detected in three layers, applied in order. Each layer passes a reduced or annotated form to the next. If a layer produces a high-confidence match, subsequent layers operate on the reduced form rather than the original.

### Layer 1 — Affix trie (automatic, high confidence)

A trie of productive prefixes and suffixes per language detects morphological relationships. The trie operates on lemmas, not surface forms.

- If an affix is detected and the resulting root **exists in the Lexicon**, a `SynonymLink` is proposed with confidence `0.9`.
- If the root does **not** exist in the Lexicon, the match is discarded entirely — this prevents false positives such as "recabar" → "cabar" (root does not exist in Spanish).
- The reduced root (without affix) is passed to Layer 2.
- If no affix is detected, the original lemma is passed to Layer 2 unchanged.

### Layer 2 — Snowball stemmer (fallback only)

Active only when no UDPipe model is available. Groups entries that share a common Snowball stem. Confidence `0.6`.

When UDPipe is active, Layer 2 acts as a **validator** for Layer 1 results: if Layer 1 reduced a form, the stemmer confirms whether the root has morphological basis before forwarding to Layer 3.

### Layer 3 — Semantic similarity + author review

Proposes candidate synonyms based on semantic distance. All proposals require explicit author confirmation via the IDE before they are accepted. Confidence `0.4` before confirmation.

### Confidence levels

| Origin | `confidence` | `confirmed` |
|---|---|---|
| Author-defined (manual) | `1.0` | `true` |
| Layer 1: affix + root exists in Lexicon | `0.9` | `false` |
| Layer 1: affix only, root not yet in Lexicon | `0.7` | `false` |
| Layer 2: shared Snowball stem (fallback) | `0.6` | `false` |
| Layer 3: semantic similarity candidate | `0.4` | `false` |

`confirmed = true` is set exclusively by the author. The compiler can be configured to emit only synonyms where `confirmed = true` or `confidence >= threshold`.

## Lexicon Lifecycle

```
1. FEED (incremental, IDE)
      Author writes text → UDPipe analyses full sentence
      → NLPToken per word → Lexicon looks up by lemma+lang
      → found:     observe_type(), accumulate frequency
      → not found: insert new entry, observe_type(), accumulate frequency
      → synonym pipeline runs on every new insertion

2. ANALYSIS (on demand or at compile time)
      apply_stemmer()    → populates stem field (fallback mode only)
      detect_phrases()   → promotes frequent n-grams to phrase entries

3. INDEXING (compile time)
      Sort entries by descending frequency per language
      Assign token IDs: top-254 → [0x0001..0x00FE]
                        rest    → [0x0100..0xFFFE]
      Emit compiler warnings for ambiguous entries

4. EMISSION (compile time)
      emit_vocab_table()   → ROM vocab_table block (ASCII strings + offsets)
      encode_string()      → serialise each game message to token ID array
      emit_huffman_tree()  → optional, for targets with spare ROM capacity
```

## Open Decisions

- [ ] JSON input format for the compiler (single file vs. multiple files per section)
- [ ] Minimum frequency threshold for an n-gram to be promoted to a phrase entry
- [ ] Compression profile per target platform (`vocab_limit`, `phrase_min_freq`)
- [ ] Concrete C++23 class `Lexicon` (container, lookup index, `record()` method)
- [ ] Concrete C++23 class `INLPBackend` and `UDPipeBackend` adapter
- [ ] Affix trie data format and per-language affix tables (JSON external files)
- [ ] Compiler warning format and reporting for ambiguous entries

# ADS — `Lexicon` System Design

## Context

The `Lexicon` is the compiler component of Adventure Designer Studio responsible for managing game vocabulary, calculating word frequencies, and producing the tokenized binary for 8-bit target platforms. It operates entirely in memory during compilation — there is no database or own persistence layer. Input is JSON; output is binary.

The system is built incrementally: the `Lexicon` grows and updates as the author writes text in the IDE, not in a single batch pass at compile time.

---

## Core Principles

- **All game text is stored as arrays of token IDs**, never as ASCII strings in ROM. A 38-character description can be reduced to 8–9 bytes.
- **The lexicon covers both output text and parser input** within the same structure, differentiated by the `role` field.
- **Entry IDs are stable at design time** — they are assigned on first insertion and never change regardless of frequency reordering. Token indices are volatile and only assigned at compile time.
- **First-class multilanguage support** — language is a primary attribute of each entry, not an external namespace. Format follows BCP-47 with region: `es_ES`, `en_US`, `de_DE`.
- **Lexical ambiguity is a first-class concern** — a single entry can carry multiple grammatical categories simultaneously, each with independent frequency tracking.

---

## NLP Backend: UDPipe

### Role

The `Lexicon` does not perform linguistic analysis itself. All tokenisation, lemmatisation, and part-of-speech tagging is delegated to **UDPipe 1.x**, a C++-native NLP pipeline developed by the Institute of Formal and Applied Linguistics at Charles University, Prague.

UDPipe processes complete sentences — not isolated words — which is essential for correct disambiguation of lexically ambiguous forms (e.g. "abierto" as verb participle vs. adjective). The context of the full sentence is required for accurate POS tagging.

### What UDPipe provides

For each word in a sentence, UDPipe returns:

| Field | Description | Example |
|---|---|---|
| `form` | Surface form as written | `"corriendo"` |
| `lemma` | Canonical dictionary form | `"correr"` |
| `upostag` | Universal Dependencies POS tag | `"VERB"` |
| `feats` | Morphological features | `"Tense=Pres\|Person=3"` |
| `head` | Dependency head index | `2` |
| `deprel` | Dependency relation | `"nsubj"` |

The `lemma` field is the primary key used by the `Lexicon` for lookup and insertion. This replaces any manual normalisation (lowercase, diacritic stripping) that would otherwise be needed — UDPipe's linguistic knowledge produces more accurate canonical forms.

### Adapter layer

The `Lexicon` never depends on `udpipe.h` directly. The UDPipe adapter translates `udpipe::word` into an `NLPToken` — an internal data carrier — before passing it to the `Lexicon`. This isolation keeps UDPipe swappable without touching the `Lexicon` code.

### Language model availability

UDPipe requires a trained model file per language (e.g. `spanish-gsd-ud-2.5.udpipe`). Models are available for all target languages: Spanish, English, German, French, Portuguese.

When no model is available for a given language, the system degrades gracefully to a rule-based fallback:

| Mode | `lemma` source | POS source | Confidence |
|---|---|---|---|
| Full UDPipe model | UDPipe lemmatiser | UDPipe tagger | `1.0` |
| Fallback (no model) | Snowball stemmer (approximate) | Not available | `0.6` |

In fallback mode, `stem` is populated instead of `lemma`, and `WordType` defaults to `Unknown` until the author classifies the entry manually.

### Supported languages (UDPipe models)

| Language | Code | Model availability |
|---|---|---|
| Spanish (Spain) | `es_ES` | Available |
| Spanish (Latin America) | `es_MX` | Available (shared model) |
| English (US) | `en_US` | Available |
| English (UK) | `en_GB` | Available (shared model) |
| German | `de_DE` | Available |
| French | `fr_FR` | Available |
| Portuguese (Portugal) | `pt_PT` | Available |
| Portuguese (Brazil) | `pt_BR` | Available (shared model) |

---

## Universal Dependencies POS Tags → WordType

UDPipe returns POS tags following the Universal Dependencies standard. These are mapped to the ADS `WordType` bitmask:

| UD UPOS | WordType | Notes |
|---|---|---|
| `VERB` | Verb | |
| `AUX` | Verb | Auxiliary verbs treated as verbs |
| `NOUN` | Noun | |
| `PROPN` | Noun | Proper nouns treated as nouns |
| `ADJ` | Adjective | |
| `ADV` | Adverb | |
| `ADP` | Preposition | |
| `DET` | Article | |
| `CCONJ` | Conjunction | Coordinating |
| `SCONJ` | Conjunction | Subordinating |
| `PRON` | Pronoun | |
| `INTJ` | Interjection | |
| `PUNCT` | Punctuation | Filtered — never inserted into the Lexicon |
| `NUM`, `SYM`, `X`, `PART` | Other | Kept, low priority |

---

## WordType: Bitmask Design

`WordType` is a `uint16_t` bitmask where each bit represents one independent grammatical category. A single entry may carry multiple bits simultaneously, reflecting genuine lexical ambiguity observed in the corpus.

### Bit layout

| Bit | Category | Example |
|---|---|---|
| 0 | Verb | `coger`, `abrir` |
| 1 | Noun | `espada`, `habitación` |
| 2 | Adjective | `oscuro`, `abierto` |
| 3 | Adverb | `rápidamente` |
| 4 | Preposition | `en`, `sobre` |
| 5 | Article | `el`, `una` |
| 6 | Conjunction | `y`, `pero` |
| 7 | Pronoun | `él`, `me` |
| 8 | Interjection | `oh`, `ay` |
| 9 | Punctuation | `.`, `,` — filtered |
| 10 | Other | numbers, symbols |
| 11–15 | — | spare for future use |

### Lexical ambiguity

When UDPipe observes a lemma with more than one POS category across the corpus, the corresponding bits are both set. The entry is flagged as ambiguous.

Each bit position maintains an independent occurrence count. The **dominant type** — the category with the highest count — is recalculated automatically on every new observation. The compiler emits a warning for every ambiguous entry, giving the author the option to split it into two distinct entries (one per WordType) at the cost of an additional token in ROM.

**Example — "abierto" (Spanish):**

```
"La puerta está abierta"  →  UDPipe: lemma="abrir", UPOS="ADJ"   → bit 2 set, count[2]++
"Abre la puerta"          →  UDPipe: lemma="abrir", UPOS="VERB"  → bit 0 set, count[0]++
```

After three ADJ observations and one VERB observation:
- `types` = `Verb | Adjective`
- `dominant_type` = `Adjective`
- Compiler warning issued

---

## Lexicon Entry Structure

Each entry represents one canonical word form within a specific language.

| Field | Type | Description |
|---|---|---|
| `id` | `uint32_t` | Stable design-time identifier. Assigned on insertion, never changes. |
| `lang` | `string` | BCP-47 language code with region (`es_ES`, `en_US`). |
| `canonical` | `string` | Canonical form from UDPipe lemma. Max 32 characters. |
| `role` | `EntryRole` | Output, Input, or Both (see below). |
| `types` | `uint16_t` | Bitmask of all observed WordType categories. |
| `dominant_type` | `uint16_t` | Single-bit WordType with highest occurrence count. |
| `type_counts` | `uint32_t[11]` | Per-bit occurrence count, indexed by bit position. |
| `upos_raw` | `string` | Raw UPOS string from UDPipe. Kept for debugging. |
| `feats` | `string` | UD morphological features string. |
| `stem` | `string` | Snowball stem. Populated only in fallback mode (no UDPipe model). |
| `synonyms` | `SynonymLink[]` | Directed synonym relationships (see Synonym Pipeline). |
| `frequency` | `float` | Normalised occurrence ratio: `raw_count / total_corpus_tokens`. |
| `raw_count` | `uint32_t` | Absolute occurrence count. Used for O(1) frequency recalculation. |
| `compiled_token` | `uint16_t` | Assigned by the compiler at index time. Invalid at design time. |

### Entry Role

| Value | Meaning |
|---|---|
| `Output` | Appears in text shown to the player |
| `Input` | Recognised by the parser (player can type it) |
| `Both` | Output and Input (default) |

---

## Canonical Node Structure

This is the authoritative definition of a `Lexicon` entry node as it exists in memory and as it is persisted to JSON. All fields listed here are present in every entry — optional fields default to the values shown.

```json
{
  "id":           "<uint32_t>",
  "lang":         "<string>",
  "canonical":    "<string>",
  "role":         "<uint8_t>",
  "types":        "<uint16_t>",
  "synonyms":     ["<uint32_t>", "..."],
  "frequency":    "<float>",
  "raw_count":    "<uint32_t>"
}
```

### Field reference

| Field | Type | Constraints | Description |
|---|---|---|---|
| `id` | `uint32_t` | Assigned on insertion. Never `0xFFFFFFFF` (reserved as INVALID). | Stable design-time identifier. Never changes across sessions or recompilations. |
| `lang` | `string` | BCP-47 with region. Max 8 chars. e.g. `"es_ES"`, `"en_US"`. | Language of this entry. Entries with the same `canonical` but different `lang` are independent entries. |
| `canonical` | `string` | Max 32 chars. Source: UDPipe lemma. Fallback: lowercase + diacritics stripped. | The single normalised word form that represents this entry. All synonyms and inflected forms resolve to this. |
| `role` | `uint8_t` | Bitmask. `0x01` = Output, `0x02` = Input, `0x03` = Both. Default: `0x03`. | Whether this entry participates in output text, parser input, or both. |
| `types` | `uint16_t` | Bitmask. One bit per grammatical category (see WordType bit layout). Default: `0x0000`. | All grammatical categories observed for this lemma across the corpus. Set automatically by the NLP pipeline; may be adjusted by the author. |
| `synonyms` | `uint32_t[]` | Each element is a valid `id` of another entry in the same language. May be empty. Forward references (target not yet inserted) are resolved at index time. | List of entry IDs that are semantically equivalent to this entry. The relationship is directional: this entry is canonical; the listed entries are variants that resolve to it. |
| `frequency` | `float` | Range `[0.0, 1.0]`. Derived: `raw_count / total_corpus_tokens`. | Normalised occurrence frequency. Recalculated on every new observation. Used for sorting and token index assignment. |
| `raw_count` | `uint32_t` | Monotonically increasing. Never decremented. | Absolute occurrence count across all text fed to the Lexicon. Allows O(1) frequency recalculation without replaying the corpus. |

### Fields not persisted to JSON

The following fields are runtime or compile-time concerns and are never written to the JSON representation:

| Field | Reason |
|---|---|
| `dominant_type` | Derived from `types` + `type_counts`. Recalculated on load. |
| `type_counts[11]` | Per-observation history. Recalculated on load or stored in a companion file. |
| `upos_raw` | Diagnostic only. Not part of the persistent contract. |
| `feats` | Transient NLP metadata. Not required after lemma and types are established. |
| `stem` | Fallback only. Not persisted when a UDPipe model is active. |
| `compiled_token` | Compile-time assignment. Regenerated on every compilation. |

### Synonym metadata

Each element in `synonyms` carries an `id` reference only. Confidence and confirmation state are stored in a companion `synonym_meta` block within the same entry:

```json
{
  "id": 3,
  "lang": "es_ES",
  "canonical": "coger",
  "role": 3,
  "types": 2,
  "synonyms": [7, 12, 45],
  "frequency": 0.12,
  "raw_count": 84,
  "synonym_meta": [
    { "target": 7,  "confidence": 1.0, "confirmed": true  },
    { "target": 12, "confidence": 0.9, "confirmed": false },
    { "target": 45, "confidence": 0.4, "confirmed": false }
  ]
}
```

`synonym_meta` is optional. If absent, all entries in `synonyms` are treated as manually confirmed (`confidence: 1.0, confirmed: true`).

### Complete example

```json
{
  "id": 3,
  "lang": "es_ES",
  "canonical": "abrir",
  "role": 3,
  "types": 5,
  "synonyms": [18, 42],
  "frequency": 0.09,
  "raw_count": 63,
  "synonym_meta": [
    { "target": 18, "confidence": 0.9, "confirmed": false },
    { "target": 42, "confidence": 1.0, "confirmed": true  }
  ]
}
```

Interpretation:

- `role: 3` → `0x03` = Both (Output + Input)
- `types: 5` → `0b00000101` = bits 0 and 2 = `Verb | Adjective` — lexically ambiguous
- `synonyms: [18, 42]` → entry 18 is an unconfirmed affix match (confidence 0.9); entry 42 was manually confirmed by the author


---

## Token ID Encoding

The `compiled_token` (`uint16_t`) is serialised into `message_pool` with variable length:

| ID Range | Bytes in ROM | Format | Usage |
|---|---|---|---|
| `0x0000` | 1 byte | `[00]` | End of message (EOL) |
| `0x0001` – `0x00FE` | **1 byte** | `[id]` | Top-254 most frequent tokens |
| `0x00FF` | — | reserved | Escape byte — never assigned as ID |
| `0x0100` – `0xFFFE` | **3 bytes** | `[FF][hi][lo]` | All remaining tokens |
| `0xFFFF` | — | sentinel | UNASSIGNED — never emitted |

The decoder on the 8-bit target reads one byte. If it is not `0xFF`, that byte is the token index directly. If it is `0xFF`, the next two bytes form the extended index. The protocol is self-describing — no state is required between tokens.

---

## In-Memory Organisation (Compiler, C++)

The `Lexicon` holds one ordered collection per active language, sorted by descending frequency. A secondary lookup index resolves any form — canonical or synonym — to its entry in O(1).

```
Lexicon
├── ordered collection per language (sorted by descending frequency)
│     "es_ES" → [entry(freq=0.12), entry(freq=0.09), ...]
│     "en_US" → [entry(freq=0.15), entry(freq=0.11), ...]
│
└── lookup index (form+lang → entry)
      "es_ES:coger"   → entry #3
      "es_ES:agarrar" → entry #3   ← synonym resolves to same entry
      "es_ES:tomar"   → entry #3   ← synonym resolves to same entry
      "en_US:take"    → entry #7
```

---

## ROM Table Separation on the Target

Although the compiler uses a single `Lexicon`, two independent structures are generated in the 8-bit target ROM:

```
Target ROM
│
├── vocab_table[]
│     Array of pointers to canonical ASCII strings.
│     Indexed directly by token ID — O(1) lookup.
│     Searched linearly when parsing player input (~200 entries, ~2ms on Z80).
│
└── message_pool[]
      Arrays of token IDs representing game texts.
      Traversed sequentially for output only.
      Never searched — direct index into vocab_table.
```

Increasing the number of unique words only affects the size of `vocab_table` in ROM — it does not affect the speed of any runtime operation.

---

## Synonym Pipeline

Synonym relationships are detected in three layers, applied in order. Each layer passes a reduced or annotated form to the next. If a layer produces a high-confidence match, subsequent layers operate on the reduced form rather than the original.

### Layer 1 — Affix trie (automatic, high confidence)

A trie of productive prefixes and suffixes per language detects morphological relationships. The trie operates on lemmas, not surface forms.

- If an affix is detected and the resulting root **exists in the Lexicon**, a `SynonymLink` is proposed with confidence `0.9`.
- If the root does **not** exist in the Lexicon, the match is discarded entirely — this prevents false positives such as "recabar" → "cabar" (root does not exist in Spanish).
- The reduced root (without affix) is passed to Layer 2.
- If no affix is detected, the original lemma is passed to Layer 2 unchanged.

### Layer 2 — Snowball stemmer (fallback only)

Active only when no UDPipe model is available. Groups entries that share a common Snowball stem. Confidence `0.6`.

When UDPipe is active, Layer 2 acts as a **validator** for Layer 1 results: if Layer 1 reduced a form, the stemmer confirms whether the root has morphological basis before forwarding to Layer 3.

### Layer 3 — Semantic similarity + author review

Proposes candidate synonyms based on semantic distance. All proposals require explicit author confirmation via the IDE before they are accepted. Confidence `0.4` before confirmation.

### Confidence levels

| Origin | `confidence` | `confirmed` |
|---|---|---|
| Author-defined (manual) | `1.0` | `true` |
| Layer 1: affix + root exists in Lexicon | `0.9` | `false` |
| Layer 1: affix only, root not yet in Lexicon | `0.7` | `false` |
| Layer 2: shared Snowball stem (fallback) | `0.6` | `false` |
| Layer 3: semantic similarity candidate | `0.4` | `false` |

`confirmed = true` is set exclusively by the author. The compiler can be configured to emit only synonyms where `confirmed = true` or `confidence >= threshold`.

---

## Lexicon Lifecycle

```
1. FEED (incremental, IDE)
      Author writes text → UDPipe analyses full sentence
      → NLPToken per word → Lexicon looks up by lemma+lang
      → found:     observe_type(), accumulate frequency
      → not found: insert new entry, observe_type(), accumulate frequency
      → synonym pipeline runs on every new insertion

2. ANALYSIS (on demand or at compile time)
      apply_stemmer()    → populates stem field (fallback mode only)
      detect_phrases()   → promotes frequent n-grams to phrase entries

3. INDEXING (compile time)
      Sort entries by descending frequency per language
      Assign token IDs: top-254 → [0x0001..0x00FE]
                        rest    → [0x0100..0xFFFE]
      Emit compiler warnings for ambiguous entries

4. EMISSION (compile time)
      emit_vocab_table()   → ROM vocab_table block (ASCII strings + offsets)
      encode_string()      → serialise each game message to token ID array
      emit_huffman_tree()  → optional, for targets with spare ROM capacity
```

---

## Open Decisions

- [ ] JSON input format for the compiler (single file vs. multiple files per section)
- [ ] Minimum frequency threshold for an n-gram to be promoted to a phrase entry
- [ ] Compression profile per target platform (`vocab_limit`, `phrase_min_freq`)
- [ ] Concrete C++23 class `Lexicon` (container, lookup index, `record()` method)
- [ ] Concrete C++23 class `INLPBackend` and `UDPipeBackend` adapter
- [ ] Affix trie data format and per-language affix tables (JSON external files)
- [ ] Compiler warning format and reporting for ambiguous entries

---

## Target Runtime Parser

The parser is the component that runs on the 8-bit target machine. It reads the player's input, identifies meaningful tokens, and produces a `ParsedCommand` structure that the condact engine uses to trigger game actions.

### Design constraints

| Constraint | Value |
|---|---|
| Maximum ROM footprint | ~600 bytes (code) + ~80 bytes (input buffer) |
| CPU | Z80 at 3.5 MHz (ZX Spectrum) |
| Lookup strategy | Linear scan over `vocab_table` (~200 entries) |
| NLP dependency | None — all linguistic analysis done at compile time |

The parser has no knowledge of UDPipe, stems, or frequencies. By the time the game runs on the target, all linguistic complexity has been resolved by the compiler into a flat `vocab_table`.

---

### ParsedCommand structure

The parser produces one or two `ParsedCommand` records per input, depending on the sentence pattern detected. Each record has four slots:

```
ParsedCommand {
    verb  : uint16_t    -- always present; 0x0000 if missing
    noun1 : uint16_t    -- primary object; 0x0000 if missing
    prep  : uint16_t    -- preposition token; 0x0000 if absent
    noun2 : uint16_t    -- secondary object; 0x0000 if absent
}
```

Slots set to `0x0000` (EOL) act as wildcards in condact matching — a condact that does not specify `prep` or `noun2` matches any value in those slots.

---

### Sentence patterns

The parser recognises exactly two structural patterns:

**Pattern A — verb + noun + preposition + noun**

```
"meter estaca en corazon"

  verb  = METER   (0x03)
  noun1 = ESTACA  (0x1A)
  prep  = EN      (0x08)
  noun2 = CORAZON (0x2C)
```

**Pattern B — verb + noun + conjunction + verb + noun**

Produces two sequential `ParsedCommand` records executed in order:

```
"coger martillo y clavar estaca"

  cmd[0]: verb=COGER,  noun1=MARTILLO, prep=0x0000, noun2=0x0000
  cmd[1]: verb=CLAVAR, noun1=ESTACA,   prep=0x0000, noun2=0x0000
```

A simple sentence with no preposition or conjunction produces a single `ParsedCommand` with `prep=0x0000` and `noun2=0x0000`.

---

### Main parser flow

```
  +---------------------------+
  |     Player presses Enter  |
  +---------------------------+
               |
               v
  +---------------------------+
  |   Read input buffer       |
  |   lowercase + normalise   |
  +---------------------------+
               |
               v
  +---------------------------+
  |   Tokenise words          |
  |   look up in vocab_table  |
  +---------------------------+
               |
               v
  +---------------------------+
  |   CONJ detected?          |
  +---------------------------+
        |              |
       yes              no
        |              |
        v              v
  +-----------+  +-----------+
  | Pattern B |  | Pattern A |
  | cmd1+cmd2 |  | single cmd|
  +-----------+  +-----------+
        |              |
        +------+-------+
               |
               v
  +---------------------------+
  |   Valid V + N found?      |
  +---------------------------+
        |              |
       yes              no
        |              |
        v              v
  +-----------+  +--------------------+
  | Build     |  | Emit "I don't      |
  | Parsed-   |  | understand" message|
  | Command[] |  +--------------------+
  +-----------+
        |
        v
  +---------------------------+
  |   Execute condacts        |
  |   match (V,N,PREP,N)      |
  +---------------------------+
        |
        v
  +---------------------------+
  |   Display response        |
  +---------------------------+
```

---

### Word tokenisation loop

The tokenisation step is the inner loop of the parser. It iterates over each word in the input, looks it up in `vocab_table`, and assigns it to the appropriate slot based on its `WordType`.

```
  +---------------------------+
  |   Read next word          |
  +---------------------------+
        |              |
    more words       end of input
        |              |
        v              v
  +-----------+     +-----+
  | Look up   |     | Exit|
  | vocab_    |     +-----+
  | table     |        |
  +-----------+        |
        |              |
     found          not found
        |              |
        v              v
  +-----------+  +-----------+
  | Check     |  | Ignore    |---+
  | WordType  |  | word      |   |
  +-----------+  +-----------+   |
     |    |            |         |
    Verb Noun         Prep/Conj  |
     |    |            |         |
     v    v            v         |
  +----+ +----+ +-------------+  |
  |verb| |noun| |flag <- PREP |  |
  |slot| |slot| |or CONJ token|  |
  |<-  | |<-  | +-------------+  |
  |tok | |tok |       |          |
  +----+ +----+       |          |
     |    |           |          |
     +----+-----------+----------+
               |
               v (loop back to next word)
```

**Slot assignment rules:**

`verb_slot` — always overwritten by the most recently seen verb. In practice the player rarely writes two verbs without a conjunction, so the last verb seen becomes the active one.

`noun_slot` — fills `noun1` first. Once a PREP or CONJ flag has been set, subsequent nouns go to `noun2`.

`prep / conj flag` — stores the token of the separator word. PREP tokens are passed through to the `ParsedCommand.prep` field. CONJ tokens trigger Pattern B: the current slots are flushed into `cmd[0]` and collection restarts for `cmd[1]`.

---

### Inflected forms in vocab_table

For languages with rich morphology (Russian, German), the compiler inserts multiple surface forms for the same token ID. All forms map to the same `TokenIndex` — the parser finds any of them and returns the same result.

```
vocab_table entry layout (per form):

  +----------+----------+----------+
  | form ptr | TokenIdx | WordType |
  | (2 bytes)| (2 bytes)| (2 bytes)|
  +----------+----------+----------+

Example — Russian noun "hammer" with N=3 forms:

  "молоток"  -> 0x1A  Noun   (nominative)
  "молотка"  -> 0x1A  Noun   (genitive)
  "молотком" -> 0x1A  Noun   (instrumental)
```

The number of forms N per word is calculated automatically at compile time:

```
N = clamp(
      floor(ROM_available / (vocab_size * bytes_per_form)),
      N_min = 1,
      N_max = forms_available_from_UDPipe
    )
```

The compiler emits a warning if `N = 1` for any language where the player is likely to write inflected forms.

---

### RAM budget (ZX Spectrum 48K, Russian)

| Component | Bytes | Notes |
|---|---|---|
| Parser code | ~600 | Fixed for all targets |
| Input buffer | ~80 | One line of player input |
| ParsedCommand × 2 | ~16 | Pattern B needs two records |
| State variables | ~20 | Slot registers, flags |
| `vocab_table` (N=3) | ~8,400 | 200 words × 3 forms × 14 bytes |
| `message_pool` | ~3,900 | 300 messages × 12 tokens avg |
| Game logic (condacts) | ~6,756 | Objects, locations, flags |
| **Total** | **~19,772** | **~47% of 42 KB available** |

Remaining headroom: ~22 KB for game content expansion.