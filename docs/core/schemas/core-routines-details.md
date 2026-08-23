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
# ADS — Meta-language (IR) reference and ROM mapping for 8-bit targets

> Design document. Status: draft for review.
> Scope: the IR layer (primitives emitted as native ASM) and its relationship to the
> firmware/ROM routines of the ZX Spectrum, Amstrad CPC, MSX/MSX2, C64, C128 and Oric Atmos.

---

## Contents

| § | Section | Covers |
|---|---|---|
| [0](#0-layer-model) | Layer model | The four layers and their conventions |
| [0bis](#0bis-author-layer--orthogonal-vocabulary-38-condacts) | Author layer | The 38 condacts across 10 families |
| [0ter](#0ter-balance-44--38) | Balance 44 → 38 | Comparison against the previous vocabulary |
| [1](#1-objects-and-inventory-23) | Objects and inventory | 23 primitives · [1.1](#11-object-table-access) access · [1.2](#12-queries--all-produce-a-condition-all-leaf) queries · [1.3](#13-mutation--no-validation-counters-maintained) mutation · [1.4](#14-iteration--single-cursor-exclusive-resource) iteration |
| [2](#2-rooms-and-movement-10) | Rooms and movement | 10 primitives |
| [3](#3-flags-and-arithmetic-12--all-leaf-no-rom) | Flags and arithmetic | 12 primitives, all leaf |
| [4](#4-control-flow-8) | Control flow | 8 primitives |
| [5](#5-text-and-output-10) | Text and output | 10 primitives |
| [6](#6-parser-6) | Parser | 6 primitives |
| [7](#7-timers-and-processes-6) | Timers and processes | 6 primitives |
| [8](#8-states-and-effects-6) | States and effects | 6 primitives |
| [9](#9-system-6) | System | 6 primitives |
| [10](#10-cross-reference-with-rom--firmware) | ROM cross-reference | [10.1](#101-the-main-finding) finding · [10.2](#102-cross-reference-table) table · [10.3](#103-why-rom-is-a-bad-idea-on-the-hot-path) critique · [10.4](#104-consequences-for-hal-design) HAL |
| [11](#11-open-questions) | Open questions | Pending decisions |

---

## 0. Layer model

| Level | Layer | Contents | Representation | Nature |
|:-:|---|---|---|---|
| 4 | **Author** | 38 condacts + range `0xE0–0xFF` | Blocks in the IDE | Closed, orthogonal |
| 3 | **Core routines** | ~30 `CR_*` | Bytecode | Project data, editable and overridable |
| 2 | **IR** ← *this document* | 87 primitives | Meta-language | Closed, machine-independent |
| 1 | **Native emission** | Z80 · 6502 · 68K · x86 · Z-machine | ASM / story file | Target-dependent |

Only **15** of the 87 level-2 primitives touch firmware on the way down to level 1
(see [§10](#10-cross-reference-with-rom--firmware)): the real portability layer is small.

**Correction to the earlier estimate.** An earlier figure of ~82 primitives with 20 in the
inventory domain was quoted. The actual count was **85**, with **23** in inventory (the
`ITER_*` sub-family is 4 primitives and had not been counted). After the author-vocabulary
revision ([§0ter](#0ter-balance-44--38)) 2 more are added: **87**.

### Cross-cutting conventions

| Concept | Definition |
|---|---|
| **Condition** | Implicit VM flag. Z80: `CY`. 6502: `C`. 68K: `Z`. Z-machine: branch-on-result. |
| **Producer** | A primitive that sets the condition. Only a producer may precede `JMP_TRUE/FALSE`. Enforced in the front-end. |
| **Leaf** | Calls nothing. Emitted inline or as a `CALL`, but **consumes no nesting level**. |
| **Depth** | Hard limit of 8. Call graph is analysable at compile time; recursion is rejected. |
| **Registers** | `R0..R3` scratch in RAM. `SYS_*` read-only from the IR, except `SYS_HEALTH`. |
| **`SYS_HEALTH`** | uint8, writable via `add`/`sub`. The engine watches it at end of turn and fires `onDeath` when it reaches 0. |

Column **C** = produces a condition. Column **D** = consumes a nesting level.

---

## 0bis. Author layer — orthogonal vocabulary (38 condacts)

> **Design decision.** DAAD is used as **coverage validation**, not as a template.
> Right question: can this vocabulary express everything DAAD could express?
> Wrong question: does it have the same condacts DAAD had?

### Why DAAD is not orthogonal

In DAAD **the editor was the compiler**: each condact was one opcode byte. Having `get`
(2 bytes) instead of `move obj, CARRIED` (3 bytes) was a real saving multiplied across
hundreds of uses. The redundancy in its vocabulary was not carelessness — it was
**compression**.

ADS has no such constraint because it has an IDE in between. **The author vocabulary and
the byte encoding are independent axes.** DAAD fused them out of necessity; here they
separate:

- The author composes in a **block editor** over a small orthogonal core.
- The emitter recognises frequent patterns and emits short specialised opcodes
  (`move obj, CARRIED` → 2-byte `GET` opcode) via peephole optimisation.

This buys DAAD's compactness without inheriting its conceptual redundancy.

### Consequences of the block editor

The block system removes any need for textual aliases and **actively favours** orthogonal
design:

| | Redundant vocabulary | Orthogonal vocabulary |
|---|---|---|
| Palette size | 44+ distinct blocks | 38 blocks, several with dropdowns |
| Readability | block name only | name + visible operands |
| Validation | after the fact, by analysis | **structural**: typed sockets |
| Adding a new destination | new condact + new block | new dropdown entry |

Typed sockets make the error **impossible** rather than detecting it later. A destination
socket that only accepts a room, container or pseudo-location can never produce an invalid
`move`.

### Flag width: a model property, not a condact property

The author declares each flag as `u8` or `u16` when creating it in the IDE. `let`, `add`,
`sub` and the comparisons are emitted as 8- or 16-bit operations according to the declared
type. **The author never writes the width.** There is no `letw`/`addw`; the block is the
same and the compiler decides. Turns, score and large counters are declared `u16`; the rest
stays `u8`.

---

### Family 1 — Data (6)

| # | Condact | Operands | Description |
|---|---|---|---|
| 01 | `let` | `flag, value\|flag` | Assign. Replaces `set` (=255), `clear` (=0) and `copy_flag`. |
| 02 | `add` | `flag, value\|flag` | Add, clamped at the declared width's ceiling. |
| 03 | `sub` | `flag, value\|flag` | Subtract, clamped at 0. |
| 04 | `and` | `flag, mask` | Bitwise. |
| 05 | `or` | `flag, mask` | Bitwise. |
| 06 | `rand` | `flag, max` | Pseudo-random 0..max−1. Replaces DAAD's `chance`. |

### Family 2 — Comparison (6) — closed and symmetric set

| # | Condact | Operands | Description |
|---|---|---|---|
| 07 | `eq` | `a, b` | Equal. Replaces `zero` (`eq f, 0`). |
| 08 | `ne` | `a, b` | Not equal. Replaces `not_zero`. |
| 09 | `lt` | `a, b` | Strictly less than. |
| 10 | `lte` | `a, b` | Less than or equal. |
| 11 | `gt` | `a, b` | Strictly greater than. |
| 12 | `gte` | `a, b` | Greater than or equal. |

> **Why `lte`/`gte` are not optional.** Without them, "score ≥ 100" is written as `gt 99`,
> which is brittle and prone to boundary errors. Worse: `>= 0` is **inexpressible** in
> unsigned arithmetic, since it would require `gt -1`. The six-way set is closed under
> negation and operand swap; a four-way set is not.

### Family 3 — Entities (3) — replaces 7 DAAD condacts

| # | Condact | Operands | Description |
|---|---|---|---|
| 13 | `move` | `obj, dest` | Move an entity to any destination. |
| 14 | `swap` | `objA, objB` | Exchange locations. |
| 15 | `where` | `flag, obj` | Entity location → flag. |

`dest` is the polymorphic location byte already defined in the model:

| Destination | Value | Block shows |
|---|---|---|
| Room | `0x00..0xFB` | room selector |
| Worn | `0xFC` WORN | "worn" |
| Inventory | `0xFD` CARRIED | "inventory" |
| Limbo | `0xFE` LIMBO | "out of play" |
| Container | object ID | container selector |

A single `move` replaces `create`, `destroy`, `move_item`, `get`, `drop`, `wear` and
`remove_worn`. It also resolves the asymmetry that was identified: **putting an object
inside a container stops being a special case**; it is the same block with a different
destination.

### Family 4 — Queries (3)

| # | Condact | Operands | Description |
|---|---|---|---|
| 16 | `at` | `ent, loc` | Is the entity at that location? With `ent = PLAYER` it covers DAAD's `at`/`not_at`; with `loc = WORN` it answers "is it being worn?", **inexpressible in DAAD**. |
| 17 | `aff` | `obj, mask` | Does it have those affordances? |
| 18 | `state` | `ent, state` | Is that state active? Works for rooms, characters, items and NPCs alike. |

### Family 5 — States (2)

| # | Condact | Operands | Description |
|---|---|---|---|
| 19 | `state_set` | `ent, state` | Apply state. Fires `onApply`. |
| 20 | `state_clear` | `ent, state` | Remove state. Fires `onRemove`. |

> Unified over **any** entity type. The compiler emits [`LOC_FLAG_SET`](#2-rooms-and-movement-10)
> or [`STATE_APPLY`](#8-states-and-effects-6) depending on the operand's type. This closes
> the gap found in the earlier reconciliation: the `onApply`/`onTick`/`onRemove` system now
> has an entry point from the author layer.

### Family 6 — World (3)

| # | Condact | Operands | Description |
|---|---|---|---|
| 21 | `goto` | `loc` | Move the player and describe. |
| 22 | `exit_set` | `loc, dir, dest` | Open, close (`dest = NONE`) or redirect an exit. Generalises `open_exit`. |
| 23 | `desc_set` | `loc, msg` | Change the active description. Generalises `swap_description`. |

### Family 7 — Output (4)

| # | Condact | Operands | Description |
|---|---|---|---|
| 24 | `print` | `msg` | Message with `{entity:field}` substitution, language selection and width fitting. |
| 25 | `name` | `ent` | Entity name with article, gender and number. Unifies `print_item_name` and `print_room_name`. |
| 26 | `describe` | `loc` | Full description, as on entering. |
| 27 | `list` | `loc\|obj` | List the contents of a room, container or inventory. |

### Family 8 — Time (2)

| # | Condact | Operands | Description |
|---|---|---|---|
| 28 | `timer_set` | `id, turns` | Arm a timer. Unit: game turns (uint8). |
| 29 | `timer_test` | `id` | Has it expired? The engine does the decrement, not the author. |

> Replaces the DAAD pattern of subtracting from a flag on every `on_turn`: faster, fewer
> blocks, and impossible to forget the decrement.

### Family 9 — Flow (5)

| # | Condact | Operands | Description |
|---|---|---|---|
| 30 | `done` | — | Action handled. Stops the event. |
| 31 | `fail` | — | Not handled. The dispatcher tries another rule. |
| 32 | `process` | `id` | Call a sub-process. |
| 33 | `skip` | `n` | Skip N effects. In blocks: visual nesting. |
| 34 | `restart` | — | Return to the top of the loop. |

### Family 10 — System (4)

| # | Condact | Operands | Description |
|---|---|---|---|
| 35 | `save` | — | Dump state. |
| 36 | `load` | — | Restore state. |
| 37 | `score` | `value` | Adjust score. |
| 38 | `end` | `victory` | End of game: 1 win, 0 lose. |

### Health: no dedicated condacts

`apply_damage` and `heal` were macros and have been dropped. They are expressed with what
already exists:

```
sub SYS_HEALTH, 10        ; damage
add SYS_HEALTH, 25        ; healing
```

**The engine watches `SYS_HEALTH` at the end of every turn** and fires `onDeath` if it
reaches 0. This works no matter how health dropped, including a direct `let` by the
author — something dedicated primitives would not guarantee.

### Range 0xE0–0xFF — platform condacts

Sound: `beep`, `sfx`, `music`, `silence`.
Graphics: `picture`, `display`, `palette`, `ink`, `paper`, `cls`.
Timing: `pause`.

Outside the core. The IDE warns when a project uses them alongside declared platforms that
do not support them.

---

## 0ter. Balance: 44 → 38

| Family | Before | Now | Δ |
|---|:-:|:-:|:-:|
| Data | 6 | 6 | = |
| Comparison | 10 | 6 | −4 |
| Entities | 7 | 3 | −4 |
| Queries | 4 | 3 | −1 |
| States | 2 | 2 | = |
| World | 3 | 3 | = |
| Output | 5 | 4 | −1 |
| Time | 0 | 2 | +2 |
| Flow | 5 | 5 | = |
| System | 4 | 4 | = |
| Player | 5 | 0 | −5 (absorbed) |
| **Total** | **44** | **38** | **−6** |

**Six fewer condacts and strictly greater coverage.** New capabilities that neither DAAD
nor the previous list could express:

- "Is it being worn?" → [`at`](#family-4--queries-3) `obj, WORN`
- Put into a container → [`move`](#family-3--entities-3--replaces-7-daad-condacts) `obj, cont`
- Correct boundary comparisons → [`gte`](#family-2--comparison-6--closed-and-symmetric-set), [`lte`](#family-2--comparison-6--closed-and-symmetric-set)
- Character, item and NPC states → [`state_set`](#family-5--states-2) on any entity
- Real timers → [`timer_set`](#family-8--time-2) / [`timer_test`](#family-8--time-2)
- Closing or redirecting exits, not just opening them → [`exit_set`](#family-6--world-3)
- 16-bit flags → declared in the model, transparent to the author

### Impact on the IR layer

Only **2 primitives** are needed; health requires none:

| New primitive | Signature | Reason |
|---|---|---|
| `LOC_SETEXIT` | `loc, dir, dest` | [`LOC_EXIT`](#2-rooms-and-movement-10) was read-only. |
| `LOC_SETDESC` | `loc, msg_id` | There was no description mutation. |

`SYS_HEALTH` joins the system register block (uint8, with a zero check at end of turn).
[`swap`](#family-3--entities-3--replaces-7-daad-condacts) needs no primitive: it expands to
[`OBJ_LOC`](#11-object-table-access) ×2 + [`INV_MOVE`](#13-mutation--no-validation-counters-maintained) ×2.

**The IR goes from 85 to 87 primitives.**

---

## 1. Objects and inventory (23)

### 1.1 Object table access

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `OBJ_LOC` | `Rd, obj` | – | – | Location byte → register. `0x00..0xFB` room, `0xFC` WORN, `0xFD` CARRIED, `0xFE` LIMBO, `0xFF` IN_CONTAINER. |
| `OBJ_SETLOC` | `obj, loc` | – | – | Raw location write. Does **not** update counters; use only during init or after `INV_*`. |
| `OBJ_PARENT` | `Rd, obj` | – | – | Container ID. Valid only when `loc == IN_CONTAINER`. |
| `OBJ_WEIGHT` | `Rd, obj` | – | – | Weight in author units (uint8). |
| `OBJ_AFF` | `Rd, obj` | – | – | Affordance bitmap (uint16). Static, author-defined. |

### 1.2 Queries — all produce a condition, all leaf

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `IS_CARRIED` | `obj` | ● | – | `loc == CARRIED`. Excludes worn items. |
| `IS_WORN` | `obj` | ● | – | `loc == WORN`. |
| `IS_HELD` | `obj` | ● | – | CARRIED **or** WORN. This is the correct query for "does the player have it?". |
| `IS_PRESENT` | `obj` | ● | – | HELD, or `loc == SYS_LOC`, or inside an open container that is present. |
| `IS_IN` | `obj, cont` | ● | – | `loc == IN_CONTAINER && parent == cont`. One level only, no recursion. |
| `AFF_TEST` | `obj, mask` | ● | – | `(affordances & mask) == mask`. Literal mask resolved at compile time. |
| `INV_FITS` | `obj` | ● | – | Count < `SYS_MAX_CARRY` **and** weight + w(obj) ≤ `SYS_MAX_W`. Heavily constant-folded. |
| `INV_FULL` | — | ● | – | `SYS_CARRIED >= SYS_MAX_CARRY`. |
| `INV_EMPTY` | — | ● | – | `SYS_CARRIED == 0`. Worn items do not count. |

### 1.3 Mutation — no validation, counters maintained

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `INV_PUSH` | `obj` | – | – | → CARRIED. `SYS_CARRIED++`, `SYS_CARRIED_W += w`. |
| `INV_POP` | `obj` | – | – | → `SYS_LOC`. Decrements counters. |
| `INV_WEAR` | `obj` | – | – | CARRIED → WORN. `SYS_CARRIED--`, `SYS_WORN++`. Weight does **not** change. |
| `INV_UNWEAR` | `obj` | – | – | WORN → CARRIED. Inverse. |
| `INV_MOVE` | `obj, loc` | – | – | Arbitrary transfer with correct counter adjustment for both source and destination. |
| `INV_INSERT` | `obj, cont` | – | – | → IN_CONTAINER, `parent = cont`. Leaves the inventory if it was there. |
| `INV_EXTRACT` | `obj` | – | – | IN_CONTAINER → CARRIED. Does not check capacity. |
| `INV_DESTROY` | `obj` | – | – | → LIMBO. Adjusts counters. Reversible via `INV_MOVE`. |

### 1.4 Iteration — single cursor, exclusive resource

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `ITER_INV` | — | – | – | Start cursor over CARRIED + WORN. |
| `ITER_LOC` | `loc` | – | – | Start cursor over a room's contents. |
| `ITER_CONT` | `cont` | – | – | Start cursor over a container's contents. |
| `ITER_NEXT` | `Rd` | ● | – | Advance. Condition false when exhausted. |

> **Re-entrancy constraint.** The cursor is single. `CR_TAKE_ALL` iterates over the room
> while calling `CR_TAKE`, which might iterate in turn. The front-end must reject any
> `ITER_*` reachable from inside an open `ITER_*` loop. Alternative: copy IDs into a buffer
> before the loop (costs N scratch bytes).

---

## 2. Rooms and movement (10)

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `LOC_EXIT` | `Rd, dir` | ● | – | Destination room in direction `dir` from `SYS_LOC`. Condition false if there is no exit. |
| `LOC_GOTO` | `loc` | – | – | Set `SYS_LOC`. Prints nothing, fires no hooks. |
| `LOC_DESC` | `loc` | – | ● | Print the long description. Calls `PRINT_MSG`, hence consumes a level. |
| `IS_AT` | `loc` | ● | – | `SYS_LOC == loc`. |
| `IS_DARK` | — | ● | – | Room is dark **and** there is no light source carried or in the room. |
| `LOC_ITEMS` | `Rd, loc` | – | – | Number of visible objects (scenery excluded). Walks the table. |
| `LOC_FLAG_TEST` | `loc, mask` | ● | – | Room attribute bitmap (visited, outdoors, safe…). |
| `LOC_FLAG_SET` | `loc, mask` | – | – | Set bits. Used by the engine to mark "visited". |
| `LOC_SETEXIT` | `loc, dir, dest` | – | – | Write an exit. `dest = NONE` closes it. Backs `exit_set`. |
| `LOC_SETDESC` | `loc, msg_id` | – | – | Change the active description token. Backs `desc_set`. |

---

## 3. Flags and arithmetic (12) — all leaf, no ROM

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `SET` | `flag, imm` | – | – | Assign a literal. |
| `LET` | `Rd, Rs` | – | – | Copy register to register. |
| `ADD` | `Rd, Rs` | – | – | Add, saturating at 255. |
| `SUB` | `Rd, Rs` | – | – | Subtract, saturating at 0. |
| `INC` | `Rd` | ● | – | Condition = overflow occurred. |
| `DEC` | `Rd` | ● | – | Condition = reached zero. Basis of timers. |
| `CMP` | `Ra, Rb` | ● | – | Equality. |
| `LT` | `Ra, Rb` | ● | – | Strictly less than, unsigned. |
| `GT` | `Ra, Rb` | ● | – | Strictly greater than, unsigned. |
| `AND` | `Rd, imm` | – | – | Bitwise. |
| `OR` | `Rd, imm` | – | – | Bitwise. |
| `RAND` | `Rd, max` | – | – | Pseudo-random 0..max−1. See [§10](#10-cross-reference-with-rom--firmware) for the per-target source. |

---

## 4. Control flow (8)

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `JMP` | `label` | – | – | Unconditional jump within the routine. |
| `JMP_TRUE` | `label` | – | – | Jump if the condition is true. Requires a producer immediately before. |
| `JMP_FALSE` | `label` | – | – | Inverse. |
| `CALL` | `CR_*` | – | ● | Call a core routine. **Consumes a level.** |
| `RET_OK` | — | – | – | Return success. Marks the action as consumed. |
| `RET_FAIL` | — | – | – | Return failure. The dispatcher may try another rule. |
| `HOOK` | `event, R0` | ● | ● | Invoke an author handler. Condition false = veto. **Consumes a level.** |
| `NOP` | — | – | – | Padding. Useful for live patching from the IDE. |

---

## 5. Text and output (10)

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `MSG` | `id` | – | ● | System message. Decompresses via Lexingine. |
| `MSG_OBJNAME` | `obj` | – | ● | Object name with article according to gender and number. |
| `MSG_LOCNAME` | `loc` | – | ● | Short room name. |
| `PRINT_MSG` | `id` | – | ● | Author message. `{entity_id:field}` substitution, compile-time language selection with EN fallback, fitting to the target's line width. |
| `PRINT_NUM` | `Rs` | – | – | Unsigned integer, no leading zeros. |
| `NEWLINE` | — | – | – | Line break with scroll control. |
| `CLS` | — | – | – | Clear the active text window. |
| `TAB` | `col` | – | – | Move to a column. |
| `ATTR` | `ink, paper` | – | – | Colour. Semantics diverge sharply per target (see [§10](#10-cross-reference-with-rom--firmware)). |
| `WINDOW` | `id` | – | – | Select a text window. Window count is target-dependent. |

> Line width per target: ZX Spectrum and MSX 32; CPC and C64/C128 40; Oric 40; Atari ST 80.

---

## 6. Parser (6)

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `INPUT_LINE` | `buf` | ● | – | Read a line with basic editing. Condition false if the line is empty. |
| `PARSE` | — | ● | ● | Tokenise and fill `SYS_VERB`, `SYS_NOUN1`, `SYS_ADJ1`, `SYS_PREP`, `SYS_NOUN2`. |
| `MATCH_VERB` | `word` | ● | – | Compare `SYS_VERB` against a literal token. |
| `MATCH_NOUN` | `word` | ● | – | Compare `SYS_NOUN1`. |
| `MATCH_ADJ` | `word` | ● | – | Compare `SYS_ADJ1`. |
| `ANYKEY` | — | – | – | Wait for a keypress. Discards the code. |

> Direction constants (`NORTH`, `SOUTH`…) do **not** go through the vocabulary: they
> resolve directly against the current room's exits. One lookup only.

---

## 7. Timers and processes (6)

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `TIMER_SET` | `id, turns` | – | – | Arm a timer. Unit: **game turns** (uint8, 0–255), never real time. |
| `TIMER_DEC` | `id` | ● | – | Decrement. Condition true on reaching 0. |
| `TIMER_TEST` | `id` | ● | – | Query without decrementing. |
| `TURN_INC` | — | – | – | Increment `SYS_TURNS_LO/HI`. Once per turn. |
| `PROCESS` | `id` | – | ● | Run a process table (equivalent to DAAD's PROCESS). |
| `DAEMON` | `id, on` | – | – | Enable/disable a per-turn background process. |

---

## 8. States and effects (6)

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `STATE_APPLY` | `ent, state` | ● | ● | Apply a state and fire `onApply`. Condition false if already active or vetoed. |
| `STATE_REMOVE` | `ent, state` | ● | ● | Remove and fire `onRemove`. |
| `STATE_TEST` | `ent, state` | ● | – | Is it active? Leaf. |
| `STATE_GET` | `Rd, ent, state` | – | – | Associated value (intensity, turns remaining). |
| `STATE_TICK` | `ent` | – | ● | Fire `onTick` for every active state. Depth hot spot. |
| `STATE_CLEAR_ALL` | `ent` | – | ● | Remove all states with their `onRemove`. Used on death or restart. |

> State ID ranges (uint8): 0 reserved · 1–8 scene · 9–58 character · 59–108 item ·
> 109–128 NPC · 129–178 user · 179–254 buffer · 255 error.

> **Depth risk.** `STATE_TICK` may invoke an `onTick` that calls core routines in turn.
> It is the most likely route to the limit of 8, alongside `CR_TAKE_ALL`.

---

## 9. System (6)

| Primitive | Signature | C | D | Description |
|---|---|:-:|:-:|---|
| `SAVE` | — | ● | ● | Dump state. Condition false on error or cancellation. |
| `LOAD` | — | ● | ● | Restore. **Recomputes** `SYS_CARRIED` and `SYS_CARRIED_W` by walking the table. |
| `RESTART` | — | – | – | Reset state from the initial image. |
| `QUIT` | — | ● | ● | Confirm and exit. Condition false if the player cancels. |
| `SCORE` | `Rd` | – | – | Current score. |
| `END` | — | – | – | End of game. Hands control back to the dispatcher. |

---

## 10. Cross-reference with ROM / firmware

### 10.1 The main finding

**Of 87 primitives, only 15 touch ROM.** The other 82 % — inventory, tables, arithmetic,
control flow, timers, states — is pure engine code, structurally identical across targets
and with no firmware dependency at all.

This has direct architectural consequences:

1. **The portability layer is small.** Porting to a new 8-bit machine means reimplementing
   15 primitives, not 87.
2. **They should be isolated.** A single `hal_<target>.asm` module with those 15 entries.
   The rest of the engine compiles identically for every Z80.
3. **Even those 15 should not always use ROM.** See [§10.3](#103-why-rom-is-a-bad-idea-on-the-hot-path).

### 10.2 Cross-reference table

Key: **⚑** = address to verify against the reference disassembly before use.

| Primitive | ZX Spectrum 48K | Amstrad CPC | MSX / MSX2 | C64 | C128 | Oric Atmos |
|---|---|---|---|---|---|---|
| `MSG` · `PRINT_MSG` · `MSG_*` (character output) | `RST 0x10` | `TXT OUTPUT &BB5A` | `CHPUT 0x00A2` | `CHROUT $FFD2` | `CHROUT $FFD2` · `PRIMM $FF7D` | direct write ⚑ |
| `PRINT_NUM` | built on `RST 0x10` | `TXT OUTPUT` | `CHPUT` | `CHROUT` | `CHROUT` | direct |
| `NEWLINE` | `RST 0x10` with `0x0D` | `TXT OUTPUT` with `&0D` | `CHPUT` with `0x0D` | `CHROUT` with `$0D` | same | direct |
| `CLS` | `0x0D6B` (CLS) | `TXT CLEAR WINDOW &BB6C` | `CLS 0x00C3` | `CHROUT` with `$93` | same | direct to `$BB80` |
| `TAB` | channel + `RST 0x10` `0x16` | `TXT SET CURSOR &BB75` | `POSIT 0x00C6` | `PLOT $FFF0` | `PLOT $FFF0` | direct |
| `ATTR` | write to attribute area `0x5800` | `TXT SET PEN &BB8A` · `TXT SET PAPER &BB90` | `CHGCLR 0x0062` | `$0286` (cursor colour) | same | **serial** attributes on screen |
| `WINDOW` | own handling | `TXT WIN ENABLE &BB66` | own handling | own handling | own handling | own handling |
| `INPUT_LINE` | `CHAN_OPEN 0x1601` + own loop ⚑ | `KM READ CHAR &BB09` | `CHGET 0x009F` | `CHRIN $FFCF` / `GETIN $FFE4` | same | ROM keyread ⚑ |
| `ANYKEY` | `KEY_SCAN 0x028E` or ports | `KM WAIT CHAR &BB06` | `CHSNS 0x009C` + `CHGET` | `GETIN $FFE4` | `GETIN $FFE4` | ⚑ |
| `SAVE` (tape) | `SA_BYTES 0x04C2` | `CAS OUT OPEN &BC8C` · `CAS OUT CHAR &BC95` | `TAPOON 0x00EA` · `TAPOUT 0x00ED` | `SAVE $FFD8` + `SETLFS $FFBA` + `SETNAM $FFBD` | same | ⚑ |
| `LOAD` (tape) | `LD_BYTES 0x0556` | `CAS IN OPEN &BC77` · `CAS IN CHAR &BC80` | `TAPION 0x00E1` · `TAPIN 0x00E4` | `LOAD $FFD5` | same | ⚑ |
| `SAVE`/`LOAD` (disk) | +3DOS `DOS_OPEN` ⚑ | AMSDOS via redirected `&BC77` | Disk BASIC / Nextor ⚑ | `OPEN $FFC0` · `CLOSE $FFC3` · `CLRCHN $FFCC` | same + `INDFET $FF74` | ⚑ |
| `RAND` (entropy source) | `FRAMES 0x5C78` | flyback counter / `&BD19` | `JIFFY 0xFC9E` | SID voice 3 `$D41B` | same | VIA counter ⚑ |
| Video sync (scroll, graphics) | `HALT` / port `0xFE` | `MC WAIT FLYBACK &BD19` | VDP interrupt | rasterline `$D012` | same | VIA |
| VRAM access (MSX only) | n/a (mapped) | n/a (mapped) | `LDIRVM 0x005C` · `WRTVRM 0x004D` · `FILVRM 0x0056` | n/a | n/a | n/a |
| Screen mode | write to `0x5C48` etc. | `SCR SET MODE &BC0E` | `CHGMOD 0x005F` | `$D011` / `$D018` | `$D011` + MMU `$FF00` | `$BFDF` / HIRES `$A000` |

### 10.3 Why ROM is a bad idea on the hot path

**ZX Spectrum.** `RST 0x10` goes through the channel system and is slow. Worse: on the 128K
and +3 the ROM can be paged out, so the engine must guarantee which bank is active before
every call. Recommendation: a private character-output routine with the font in RAM. Costs
~120 bytes, with a large gain in `CR_LOOK` and inventory listings.

**Amstrad CPC.** The jumpblock is stable and well documented, but requires the firmware
enabled and low RAM intact. If RAM is paged for text, the firmware must be re-enabled on
each return. `TXT OUTPUT` is convenient for the prototype; it should be replaced by direct
screen writes once the engine has stabilised.

**MSX.** The BIOS forces slot switching, at a real per-call cost. `LDIRVM` is worth keeping
because it encapsulates the VDP protocol correctly. `CHPUT` is not.

**C64 / C128.** `CHROUT` is reasonably fast, but interprets PETSCII control characters,
which clashes with an engine that already does its own formatting. Direct writes to `$0400`
with a private translation table are more predictable. On the C128, `INDFET $FF74` is
essential if text lives in another bank: it is the only clean way to read through the MMU.

**Oric Atmos.** The least documented target of the set; ROM entries marked ⚑ must be
verified before use. What is solid: the text screen at `$BB80`–`$BFDF` (40×28) and HIRES at
`$A000`. Attributes are **serial**, not per cell: an attribute byte occupies a character
position and affects the rest of the line. This breaks the semantics of
[`ATTR`](#5-text-and-output-10) relative to every other target and must be modelled
explicitly in the output layer, not patched in the emitter.

### 10.4 Consequences for HAL design

The 15 machine-dependent primitives group into 5 services:

| Service | Primitives | Recommended strategy |
|---|---|---|
| **Character output** | [`MSG`](#5-text-and-output-10), [`PRINT_MSG`](#5-text-and-output-10), [`PRINT_NUM`](#5-text-and-output-10), [`NEWLINE`](#5-text-and-output-10), `MSG_*` | Private routine on every target. |
| **Screen control** | [`CLS`](#5-text-and-output-10), [`TAB`](#5-text-and-output-10), [`ATTR`](#5-text-and-output-10), [`WINDOW`](#5-text-and-output-10) | ROM on CPC and MSX; direct on Spectrum, C64/C128, Oric. |
| **Input** | [`INPUT_LINE`](#6-parser-6), [`ANYKEY`](#6-parser-6) | ROM for raw scanning, private line editing. |
| **Storage** | [`SAVE`](#9-system-6), [`LOAD`](#9-system-6) | **Always ROM.** Reimplementing tape or disk gains nothing. |
| **Entropy and sync** | [`RAND`](#3-flags-and-arithmetic-12--all-leaf-no-rom), flyback | ROM or system counter, either way. |

[`SAVE`](#9-system-6)/[`LOAD`](#9-system-6) is the one case where ROM wins outright: the
tape protocol is delicate, well solved in firmware, and on no hot path.

---

## 11. Open questions

- [ ] Verify the entries marked ⚑ against reference disassemblies, particularly the entire
  Oric Atmos block.
- [ ] Decide whether the `ITER_*` cursor is declared an exclusive resource (static
  validation) or whether buffer copying is allowed (scratch cost).
- [ ] Measure the real bytecode size of 5–6 core routines and replace the 70 B/routine
  estimate with the measured value.
- [ ] Fix the emitter's inline vs. `CALL` threshold for leaf primitives. Proposed starting
  point: inline up to 3 uses, `CALL` beyond that, except in hot loops.
- [ ] Specify the Oric's serial attribute model as a first-class case in the output layer.
- [ ] Decide whether `mul` / `div` join the data family. On 8 bits they are not free
  (~40-byte routine on Z80, worse on 6502) and no classic mechanic requires them.
  Starting position: **out**, until a real case justifies them.
- [ ] Define the emitter's peephole pattern catalogue: which orthogonal combinations
  collapse to a short opcode (`move obj, CARRIED` → `GET`) and at what frequency
  threshold.
- [ ] Specify the block editor's socket types and their correspondence to each condact's
  operands. This is what turns validation structural.
- [ ] Review whether [`swap`](#family-3--entities-3--replaces-7-daad-condacts) deserves its
  own primitive or stays a 4-instruction expansion.