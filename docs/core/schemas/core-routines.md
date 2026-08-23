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

# Core Routines

Core Routines are small, platform-specific ASM implementations that
form the foundation of all game logic. They are analogous to the
Condactos in DAAD: a closed vocabulary of primitives that authors
chain together via the event system's `calls_stack` to create complex
behaviors.

## Organization

Core Routines are divided into two tiers:

- **Tier 1 (Primitives):** Low-level operations on memory, state, and
  conditionals
- **Tier 2 (Complex Routines):** High-level game actions built by
  composing Tier 1 primitives

This document specifies **Tier 1** in detail.

---

# Tier 1: Primitive Routines

Tier 1 routines are the building blocks. They operate on entity state,
memory, comparison, and repetition. All Tier 1 routines follow a
consistent calling and return convention.

## Design-Time Representation

Parameters are **entity references** in the form `entity_id:field_name`,
where:

- `entity_id` is a valid project identifier (e.g., `scene_crypt`,
  `item_torch`, `player`, `npc_guard`)
- `field_name` is a state or affordance field defined on that entity
  (e.g., `dark`, `durability`, `life`)
- Immediate values are written as hex literals: `0xFF`, `0x00`, `0x42`

**Example:** `MOV 0x01, scene_crypt:dark` (set scene_crypt's dark field
to 1)

## Compile-Time Transformation

During compilation, entity references are resolved to memory addresses
via ASM variables:

- `scene_crypt:dark` → `[asm_var_scene_crypt_state + offset_dark]`
- The compiler generates indirect addressing code or lookup tables to
  resolve symbolic names to actual memory locations at link time

## Calling Convention

All Tier 1 routines return a **status code** via a platform-specific
register:

| Platform | Return Register | Convention |
|----------|-----------------|------------|
| Z80 (ZX Spectrum, Amstrad CPC, MSX) | A | 0 = success, non-zero = fail |
| 6502 (C64) | A | 0 = success, non-zero = fail |
| 68000 (Atari ST) | D0 | 0 = success, non-zero = fail |

Comparison operations (`CMP`, `GT`, `LT`, `GE`, `LE`) additionally set
an implicit **comparison flag** that persists until the next comparison.
This flag is used by Tier 2 control-flow routines (e.g., `JMP_NZ`).

## Runtime Semantics

All routines execute synchronously within a single game tick unless
otherwise noted. Routines in a `calls_stack` execute sequentially;
control flow (branching on success/failure or comparison results) is
handled by Tier 2 routines.

---

## Routines

### NOP — No Operation

**Purpose:** Consume a game turn without performing any action. Used
for delays or padding in event sequences.

**Format:** `NOP`

**Parameters:** None

**Returns:** 0 (always succeeds)

**Examples:**
```
NOP                                     — Consume one turn
```

---

### MOV — Move Value

**Purpose:** Copy a value from a source to a destination. If the source
is wider than the destination, the value is truncated to fit.

**Format:** `MOV <source>, <destination>`

**Parameters:**
- `<source>`: Entity reference (`entity_id:field`) or immediate value
  (hex literal)
- `<destination>`: Entity reference (`entity_id:field`)

**Returns:** 0 on success, 1 on failure (e.g., if destination doesn't
exist or is read-only)

**Notes:** The destination must be writable. If source and destination
are different widths, truncation is silent (no error).

**Examples:**
```
MOV 0x01, scene_crypt:dark
  — Set scene_crypt dark state to 1
MOV player:life, temp_register
  — Copy player life to a temporary
MOV 0x00, item_torch:durability
  — Reset torch durability to 0
```

---

### LD — Load Value

**Purpose:** Load an immediate value into a register or temporary
location.

**Format:** `LD <value>, <destination>`

**Parameters:**
- `<value>`: Immediate hex literal (e.g., `0xFF`, `0x42`)
- `<destination>`: Register or temporary location (entity reference or
  temporary ID)

**Returns:** 0 on success, 1 on failure

**Notes:** Primarily used to prepare values for subsequent operations
(e.g., before `MOV` or `CMP`).

**Examples:**
```
LD 0xFF, temp_value
  — Load 0xFF into temp_value
LD 0x00, player:status_flags
  — Clear player status flags
```

---

### CMP — Compare

**Purpose:** Compare two values and set the comparison flag. Returns -1
if source1 < source2, 0 if equal, 1 if source1 > source2. The result
is stored in the implicit comparison flag for use by conditional
branches.

**Format:** `CMP <source1>, <source2>`

**Parameters:**
- `<source1>`: Entity reference or immediate value
- `<source2>`: Entity reference or immediate value

**Returns:** Sets comparison flag (-1, 0, or 1); always returns 0 as
exit status

**Notes:** The comparison flag is implicit and persists until the next
`CMP` operation. Use `GT`, `LT`, `GE`, or `LE` after `CMP` to branch on
the result.

**Examples:**
```
CMP player:life, 0x00
  — Is player dead?
CMP item_torch:durability, 0x00
  — Is torch depleted?
CMP inventory:weight, inventory:capacity
  — Is inventory full?
```

---

### GT — Greater Than

**Purpose:** Test if source1 > source2 (based on the last `CMP` result).
Returns 1 if true, 0 if false.

**Format:** `GT <source1>, <source2>`

**Parameters:**
- `<source1>`: Entity reference or immediate value
- `<source2>`: Entity reference or immediate value

**Returns:** 1 if source1 > source2, 0 otherwise

**Notes:** Internally calls `CMP` and evaluates the result. Sets the
comparison flag.

**Examples:**
```
GT player:life, 0x00
  — Is player alive?
GT item_torch:durability, 0x05
  — Does torch have more than 5 turns left?
```

---

### LT — Less Than

**Purpose:** Test if source1 < source2. Returns 1 if true, 0 if false.

**Format:** `LT <source1>, <source2>`

**Parameters:**
- `<source1>`: Entity reference or immediate value
- `<source2>`: Entity reference or immediate value

**Returns:** 1 if source1 < source2, 0 otherwise

**Notes:** Internally calls `CMP` and evaluates the result. Sets the
comparison flag.

**Examples:**
```
LT player:life, 0x0A
  — Is player below 10 HP?
LT inventory:weight, inventory:capacity
  — Can inventory hold more?
```

---

### GE — Greater or Equal

**Purpose:** Test if source1 >= source2. Returns 1 if true, 0 if false.

**Format:** `GE <source1>, <source2>`

**Parameters:**
- `<source1>`: Entity reference or immediate value
- `<source2>`: Entity reference or immediate value

**Returns:** 1 if source1 >= source2, 0 otherwise

**Notes:** Internally calls `CMP` and evaluates the result. Sets the
comparison flag.

**Examples:**
```
GE player:life, 0x01
  — Is player at least barely alive?
GE item_torch:durability, 0x03
  — Does torch have at least 3 turns?
```

---

### LE — Less or Equal

**Purpose:** Test if source1 <= source2. Returns 1 if true, 0 if false.

**Format:** `LE <source1>, <source2>`

**Parameters:**
- `<source1>`: Entity reference or immediate value
- `<source2>`: Entity reference or immediate value

**Returns:** 1 if source1 <= source2, 0 otherwise

**Notes:** Internally calls `CMP` and evaluates the result. Sets the
comparison flag.

**Examples:**
```
LE player:life, 0x00
  — Is player dead or dying?
LE inventory:items_count, 0x01
  — Inventory empty or single item?
```

---

## Control Flow Routines

Control flow routines branch execution based on the status of the last
operation or the comparison flag from the last comparison.

### JMP_NZ — Jump if Not Zero (Error)

**Purpose:** Jump to a target routine if the last operation failed (return
status is non-zero). Used to implement error handling and conditional
branching in `calls_stack`.

**Format:** `JMP_NZ <target_routine_id>`

**Parameters:**
- `<target_routine_id>`: The ID or name of the routine to jump to if
  last operation failed

**Returns:** 0 (always succeeds; execution continues from target or next
routine)

**Notes:**
- Checks the platform register from the last operation.
- If register != 0 (failure), execution jumps to `target_routine_id`.
- If register == 0 (success), execution continues normally to the next
  routine in `calls_stack`.
- Does not modify the comparison flag.

**Examples:**
```
PICK_UP sword
JMP_NZ error_handler
  — Jump to error_handler if pick up fails
PRINT_MSG "Sword acquired"
error_handler:
PRINT_MSG "Could not pick up sword"
```

---

### JMP_Z — Jump if Zero (Success)

**Purpose:** Jump to a target routine if the last operation succeeded
(return status is zero). Useful for branching on successful completion.

**Format:** `JMP_Z <target_routine_id>`

**Parameters:**
- `<target_routine_id>`: The ID or name of the routine to jump to if
  last operation succeeded

**Returns:** 0 (always succeeds)

**Notes:**
- Checks the platform register from the last operation.
- If register == 0 (success), execution jumps to `target_routine_id`.
- If register != 0 (failure), execution continues normally.
- Does not modify the comparison flag.

**Examples:**
```
CMP player:life, 0x00
JMP_Z player_dead
  — Jump if player is alive (comparison == 0)
PRINT_MSG "Player survives"
player_dead:
PRINT_MSG "Player is dead"
```

---

### JMP_GT — Jump if Last Comparison Greater Than

**Purpose:** Jump to a target routine if the comparison flag indicates
source1 > source2 from the last `CMP` operation.

**Format:** `JMP_GT <target_routine_id>`

**Parameters:**
- `<target_routine_id>`: Routine to jump to if flag > 0

**Returns:** 0 (always succeeds)

**Notes:** Reads the implicit comparison flag set by the last `CMP`
operation. Does not affect the flag.

---

### JMP_LT — Jump if Last Comparison Less Than

**Purpose:** Jump if the comparison flag indicates source1 < source2.

**Format:** `JMP_LT <target_routine_id>`

**Parameters:**
- `<target_routine_id>`: Routine to jump to if flag < 0

**Returns:** 0 (always succeeds)

---

### JMP_GE — Jump if Last Comparison Greater or Equal

**Purpose:** Jump if the comparison flag indicates source1 >= source2.

**Format:** `JMP_GE <target_routine_id>`

**Parameters:**
- `<target_routine_id>`: Routine to jump to if flag >= 0

**Returns:** 0 (always succeeds)

---

### JMP_LE — Jump if Last Comparison Less or Equal

**Purpose:** Jump if the comparison flag indicates source1 <= source2.

**Format:** `JMP_LE <target_routine_id>`

**Parameters:**
- `<target_routine_id>`: Routine to jump to if flag <= 0

**Returns:** 0 (always succeeds)

---

### RPT — Repeat

**Purpose:** Execute a single routine multiple times in succession.

**Format:** `RPT <routine_id>, <times>`

**Parameters:**
- `<routine_id>`: The mnemonic or ID of a core routine or user-defined
  routine
- `<times>`: Number of repetitions (immediate value or entity reference)

**Returns:** 0 on success, 1 on failure (e.g., routine not found or
times is invalid)

**Notes:**

- Repeats the routine with the same parameters each time (the parameters
  come from the context where `RPT` is called, not from `RPT` itself).
- If the repeated routine fails, subsequent repetitions still execute
  (no early exit).
- Used to implement loops within the `calls_stack` without requiring
  separate control-flow syntax.

**Examples:**
```
RPT PICK_UP, 3
  — Pick up the same item 3 times
RPT NOP, player:life
  — Consume N turns equal to player life
```

---

## Summary Table

| Mnem. | Name | Use | Returns |
|-------|------|-----|---------|
| NOP | No Operation | Padding | 0 |
| MOV | Move Value | Copy state | 0/1 |
| LDV | Load Value | Prep values | 0/1 |
| CMP | Compare | Test relation | 0 (flag -1/0/1) |
| GT | Greater Than | Test (>) | 1/0 |
| LT | Less Than | Test (<) | 1/0 |
| GE | Greater Eq | Test (>=) | 1/0 |
| LE | Less Eq | Test (<=) | 1/0 |
| JMP_NZ | Jump Not Zero | Branch on fail | 0 |
| JMP_Z | Jump Zero | Branch on success | 0 |
| JMP_GT | Jump Greater | Branch on > | 0 |
| JMP_LT | Jump Less | Branch on < | 0 |
| JMP_GE | Jump Gte | Branch on >= | 0 |
| JMP_LE | Jump Lte | Branch on <= | 0 |
| RPT | Repeat | Loop N times | 0/1 |

---

## Design Notes

**Return status is implicit:** Every routine writes a status code to
the platform register (0 = success, non-zero = fail). The caller (or
Tier 2 control flow routines) can inspect this via `JMP_NZ` or similar.

**Comparison flag is implicit:** `CMP` sets a comparison flag (-1/0/1)
that persists until the next comparison. Subsequent calls to `GT`, `LT`,
`GE`, or `LE` read this flag.

**Control flow in Tier 1:** Branching on success/failure or comparison
results is handled by Tier 1 control-flow routines (`JMP_NZ`, `JMP_Z`,
`JMP_GT`, `JMP_LT`, `JMP_GE`, `JMP_LE`). Combined with comparison and
comparison-flag operations, these enable full conditional logic.

**Entity references:** All parameters that refer to game entities use
the `entity_id:field` notation, which the IDE resolves at compile time
to ASM indirect addressing.

**Sequential execution:** Tier 1 routines in a `calls_stack` execute in
array order unless a Tier 2 control-flow routine branches execution.

---

# Tier 2: Complex Routines

Tier 2 routines are high-level game actions built by composing Tier 1
primitives. They encapsulate common patterns: inventory management, item
manipulation, character state, scene transitions, and NPC interaction.
Tier 2 routines return status via the platform register (same calling
convention as Tier 1).

## Inventory Routines

### PICK_UP — Pick Up Item

**Purpose:** Add an item to the player's inventory (the backpack). Fails
if inventory is full or item is not takeable.

**Format:** `PICK_UP <item_id>`

**Parameters:**
- `<item_id>`: The ID of the item to pick up

**Returns:** 0 on success, 1 on failure (inventory full, item not
takeable, etc.)

**Side Effects:**
- Decrements `inventory:capacity` by 1
- Increases `inventory:weight` by the item's weight
- Moves item from current location to player inventory

**Examples:**
```
PICK_UP item_torch
  — Pick up the torch
```

---

### DROP — Drop Item

**Purpose:** Remove an item from the player's inventory and place it in
the current scene.

**Format:** `DROP <item_id>`

**Parameters:**
- `<item_id>`: The ID of the item to drop

**Returns:** 0 on success, 1 on failure (item not in inventory, etc.)

**Side Effects:**
- Increments `inventory:capacity` by 1
- Decrements `inventory:weight` by the item's weight
- Moves item from inventory to current scene

**Examples:**
```
DROP item_torch
  — Drop the torch in current scene
```

---

### LIST_INV — List Inventory Items

**Purpose:** Return a list of all items currently in the player's
inventory. Used for display or programmatic checking.

**Format:** `LIST_INV`

**Parameters:** None

**Returns:** 0 (always succeeds); items are stored in a system buffer
accessible to game engine or display routines

**Examples:**
```
LIST_INV
  — Populate inventory list for display
PRINT_MSG "Inventory:"
```

---

### CHECK_CAP — Check Inventory Capacity

**Purpose:** Test if the inventory has space for one more item. Returns
1 if full, 0 if space available.

**Format:** `CHECK_CAP`

**Parameters:** None

**Returns:** 0 if capacity available, 1 if full (or no inventory system)

**Examples:**
```
CHECK_CAP
JMP_NZ inventory_full
  — Jump if inventory is full
PICK_UP item_key
inventory_full:
PRINT_MSG "Inventory full"
```

---

## Item Routines

### USE_ITM — Use Item

**Purpose:** Use an item from the inventory. Triggers the item's `onUse`
event or default behavior. Item remains in inventory unless consumed.

**Format:** `USE_ITM <item_id>`

**Parameters:**
- `<item_id>`: The ID of the item to use

**Returns:** 0 on success, 1 on failure (item not in inventory, not
useable, etc.)

**Side Effects:** Fires the item's `onUse` event, which may modify game
state, spawn effects, etc. Item is consumed if marked as consumable.

**Examples:**
```
USE_ITM item_key
  — Use the key (fires onUse event)
```

---

### DESTROY_ITM — Destroy Item

**Purpose:** Permanently remove an item from the game. If the item is in
inventory, also updates capacity and weight.

**Format:** `DESTROY_ITM <item_id>`

**Parameters:**
- `<item_id>`: The ID of the item to destroy

**Returns:** 0 on success, 1 on failure (item not found, etc.)

**Side Effects:** If item is in inventory, frees capacity and weight.

**Examples:**
```
DESTROY_ITM item_broken_torch
  — Remove the broken torch from game
```

---

### MOVE_ITM — Move Item to Container/Scene

**Purpose:** Relocate an item from its current location to a target
container, scene, or NPC inventory.

**Format:** `MOVE_ITM <item_id>, <target_id>, <target_type>`

**Parameters:**
- `<item_id>`: The item to move
- `<target_id>`: The destination (scene, NPC, container, or `player` for
  inventory)
- `<target_type>`: `scene`, `npc`, `container`, or `player`

**Returns:** 0 on success, 1 on failure (item not found, target full, etc.)

**Side Effects:** Updates item location; adjusts inventory stats if
source or target is the player's inventory.

**Examples:**
```
MOVE_ITM item_gold, scene_treasure, scene
  — Move gold to treasure scene
MOVE_ITM item_key, npc_guard, npc
  — Give key to guard NPC
```

---

## Character Routines

### HEAL — Restore Life Points

**Purpose:** Increase the player's life points by a specified amount.

**Format:** `HEAL <amount>`

**Parameters:**
- `<amount>`: Immediate value or entity reference (e.g., `0x0A`,
  `item:healing_power`)

**Returns:** 0 on success, 1 on failure

**Side Effects:** Increases `player:life` by amount (capped at max).

**Notes:** If life exceeds max, it is clamped to max.

**Examples:**
```
HEAL 0x05
  — Restore 5 life points
```

---

### DAMAGE — Reduce Life Points

**Purpose:** Decrease the player's life points by a specified amount.

**Format:** `DAMAGE <amount>`

**Parameters:**
- `<amount>`: Immediate value or entity reference

**Returns:** 0 on success, 1 on failure

**Side Effects:** Decreases `player:life` by amount. If life reaches 0,
may trigger game-over event.

**Examples:**
```
DAMAGE 0x02
  — Reduce life by 2 points
```

---

### GET_LIFE — Get Current Life Points

**Purpose:** Retrieve the player's current life value into a temporary
register or location for inspection.

**Format:** `GET_LIFE <destination>`

**Parameters:**
- `<destination>`: Where to store the life value (temp register or
  entity reference)

**Returns:** 0 on success, 1 on failure

**Examples:**
```
GET_LIFE temp_life
CMP temp_life, 0x00
JMP_NZ player_alive
  — Check if player is dead
player_alive:
PRINT_MSG "Player is alive"
```

---

### DIE — Kill Player

**Purpose:** End the game: set player life to 0 and trigger game-over
event.

**Format:** `DIE`

**Parameters:** None

**Returns:** 0 (always succeeds)

**Side Effects:** Sets `player:life` to 0, fires game-over event,
halts further game updates.

**Examples:**
```
DIE
  — Player is dead; game over
```

---

## Scene Routines

### MOVE_TO — Move Player to Scene

**Purpose:** Transition the player to a new scene. Fires the destination
scene's `onEnter` event on arrival.

**Format:** `MOVE_TO <scene_id>`

**Parameters:**
- `<scene_id>`: The ID of the destination scene

**Returns:** 0 on success, 1 on failure (scene not found, etc.)

**Side Effects:**
- Updates player's current scene
- Fires `onEnter` event on the destination scene
- May update visibility, light, and ambient descriptions

**Examples:**
```
MOVE_TO scene_crypt
  — Move player to the crypt
```

---

### SWAP_DESC — Swap Description

**Purpose:** Replace the description, long description, odor, or sound
of a scene, item, or NPC at runtime. Used to reflect state changes
(e.g., torch lit vs. unlit).

**Format:** `SWAP_DESC <target_id>, <target_type>, <field>, <new_text_id>`

**Parameters:**
- `<target_id>`: Scene, item, or NPC ID
- `<target_type>`: `scene`, `item`, or `npc`
- `<field>`: `description`, `long_description`, `odor`, or `sound`
- `<new_text_id>`: ID of the new text string to use

**Returns:** 0 on success, 1 on failure

**Side Effects:** Updates the entity's description; changes what the
player sees on next LOOK or revisit.

**Examples:**
```
SWAP_DESC scene_crypt, scene, description, dark_crypt_desc
  — Change scene description to dark version
```

---

## NPC Routines

### GIVE_TO — Give Item to NPC

**Purpose:** Transfer an item from the player's inventory to an NPC. NPC
may have its own inventory/container.

**Format:** `GIVE_TO <item_id>, <npc_id>`

**Parameters:**
- `<item_id>`: The item to give
- `<npc_id>`: The NPC to give to

**Returns:** 0 on success, 1 on failure (item not in inventory, NPC full,
etc.)

**Side Effects:** Removes item from player inventory, adds to NPC
inventory. Updates capacity and weight.

**Examples:**
```
GIVE_TO item_key, npc_guard
  — Give key to guard
```

---

### TAKE_FROM — Take Item from NPC

**Purpose:** Take an item from an NPC's inventory and add it to the
player's inventory.

**Format:** `TAKE_FROM <item_id>, <npc_id>`

**Parameters:**
- `<item_id>`: The item to take
- `<npc_id>`: The NPC to take from

**Returns:** 0 on success, 1 on failure (item not on NPC, player
inventory full, etc.)

**Side Effects:** Removes item from NPC inventory, adds to player
inventory.

**Examples:**
```
TAKE_FROM item_gold, npc_merchant
  — Take gold from merchant
```

---

## Game I/O Routines

### PRINT_MSG — Print Message

**Purpose:** Display a formatted, dynamically-composed message to the
player. Handles text compression (lexingine), variable substitution
(entity names, values), localization, and platform constraints.

**Format:** `PRINT_MSG <message_id>, [<substitution_param1>, ...]`

**Parameters:**
- `<message_id>`: The ID of the message template
- `<substitution_param_N>` (optional): Entity references for placeholder
  substitution

**Returns:** 0 on success, 1 on failure

**Notes:** `PRINT_MSG` is a complex routine requiring detailed
specification. See **Special Core Routines** section for full design,
message template syntax, special codes, and implementation details.

**Examples:**
```
PRINT_MSG msg_torch_found
  — Simple message
PRINT_MSG msg_picked_up, item_torch
  — Message with item name substitution
```

---

### PLAY_SND — Play Sound

**Purpose:** Play a sound effect or audio cue. Only works if the target
platform supports audio.

**Format:** `PLAY_SND <sound_id>`

**Parameters:**
- `<sound_id>`: The ID or address of the sound resource

**Returns:** 0 on success, 1 on failure (platform has no audio, sound
not found, etc.)

**Notes:** On platforms without audio hardware, returns 1 (fail) but
does not halt execution.

**Examples:**
```
PLAY_SND "door_open"
  — Play door opening sound
```

---

## Tier 2 Summary

Tier 2 routines compose Tier 1 primitives to implement game-specific
actions: inventory, items, character, scenes, and NPCs. They accept
entity references and follow the same calling convention (return via
platform register). By combining Tier 1 control flow (`JMP_*`), data
operations (`MOV`, `LDV`), and comparison (`CMP`), authors can implement
complex event logic without separate scripting languages.

---

# Compile-Time Optimizations

## ROM Routine Mapping

To minimize generated code size on constrained platforms, the IDE
maintains a built-in catalog of ROM entry points for each target
platform. At compile time, when a core routine is encountered, the IDE
checks if a ROM implementation exists for that routine on the target
platform.

### ROM Routine Catalog (IDE-Internal)

The IDE contains platform-specific mappings of routines to ROM addresses:

**Z80 Platforms (ZX Spectrum, Amstrad CPC, MSX):**
```
PRINT_MSG → 0x203C (ZX Spectrum character output)
PRINT_MSG → 0xBBC0 (Amstrad CPC character output)
PRINT_MSG → 0xA0A0 (MSX character output)
PLAY_SND → [platform-specific or unavailable]
```

**6502 Platform (C64):**
```
PRINT_MSG → 0xFFD2 (CHROUT — character output)
PLAY_SND → 0xFF1E (audio/input routine)
```

**68K Platform (Atari ST):**
```
PRINT_MSG → [BIOS trap or system call]
PLAY_SND → [audio hardware or BIOS]
```

### Wrapper Generation Strategy

When compiling a core routine that has a ROM implementation:

1. **Parameter Marshaling:** IDE generates minimal code to place routine
   parameters into registers/memory the ROM routine expects

2. **ROM Call:** IDE emits a `JSR <rom_address>` (Z80: `CALL`, 6502:
   `JSR`, 68K: `BSR`/`JSR`)

3. **Return Status:** IDE retrieves return status from the register(s)
   where the ROM routine leaves it (platform-specific)

4. **Fallback:** If no ROM routine exists, IDE generates the full ASM
   implementation from scratch

### Example: PRINT_MSG on ZX Spectrum

**Full implementation (no ROM used):** ~100+ bytes

**ROM-optimized implementation:**
```asm
; Decompress message via lexingine
; ...
; For each character in decompressed text:
LD A, [char_ptr]        ; Load character
CALL 0x203C             ; Call ZX ROM print routine
INC [char_ptr]          ; Advance pointer
; ... repeat until end of message
; Return status to caller
```

**Result:** ~30-40 bytes (wrapper only)

### Compile-Time Decision

The IDE configuration specifies which ROM routines to use (or `native`
for full implementation). This is set per-project or per-target-platform.
At compile time:

1. IDE checks: Does routine have ROM for target platform?
2. If yes: Generate wrapper
3. If no: Generate full ASM implementation
4. Compiler generates code and embeds ROM addresses

### Benefits

- Smaller ROM footprint on 8-bit targets
- Faster execution (ROM code is optimized)
- Less maintenance (rely on proven platform ROM)

### Drawbacks

- Limited customization (ROM behavior is fixed)
- Platform-specific (each platform needs separate mapping)
- Must handle ROM calling convention differences

Some core routines are significantly more complex due to their interaction
with multiple subsystems (text compression, localization, dynamic
composition, platform constraints). These require detailed specification
beyond the standard format.

## PRINT_MSG — Print Message (Complex Specification)

### Purpose

Display a formatted, dynamically-composed message to the player. Handle
text compression (via lexingine token IDs), variable substitution (entity
names, numeric values), localization (language selection), and platform
constraints (8-bit memory, display width).

### Design Constraints

**Text Compression:**
- Messages are stored as `uint16_t` token IDs in the lexingine text bank
- Each token may represent a word, phrase, or special code
- `0xFF` is the escape byte for multi-byte sequences
- Decompression happens at runtime on the target platform

**Variable Substitution:**
- Messages may contain placeholders for dynamic data
- Examples: `{item:name}`, `{player:life}`, `{npc:name}`
- At compile time, the IDE resolves entity references
- At runtime, the engine substitutes actual values

**Localization:**
- Messages exist in multiple language versions (BCP-47 codes)
- The engine has a current language setting (`player:language` or global)
- `PRINT_MSG` selects the appropriate language variant

**Platform Constraints:**
- 8-bit targets have ~42 KB available RAM
- Display width and text buffer size vary by platform:
    - ZX Spectrum, MSX: 32 characters
    - Amstrad CPC, C64: 40 characters
    - Atari ST: 80 characters
- Text must be word-wrapped and queued for display
- No persistent text buffers; messages are streamed or queued

**Special Codes:**
- Messages may contain embedded codes for pauses, newlines, and optional
  platform-specific features (color, sound).
- Examples: `[PAUSE:500]` (wait 500ms), `[NEWLINE]`, `[COLOR:AMBER]`,
  `[SFX:beep]`
- Each target platform (Z80, 6502, 68K) implements these codes
  according to its capabilities. Codes unsupported by a platform are
  gracefully ignored.
- Core codes (PAUSE, NEWLINE) are supported on all platforms.
- Optional codes (COLOR, SFX) depend on platform hardware.

### Format

```
PRINT_MSG <message_id>, [<substitution_param1>, ...]
```

### Parameters

- `<message_id>`: The unique ID of the message template in the text bank
  (e.g., `msg_picked_up_item`, `msg_npc_speaks`)
- `<substitution_param_N>` (optional): Entity references or values to
  substitute into placeholders (e.g., `item_torch:name`, `player:life`)

### Returns

0 on success, 1 on failure (message not found, decompression error, etc.)

### Compile-Time Behavior

1. **Message Resolution:** IDE looks up `message_id` in the text bank
2. **Placeholder Detection:** IDE scans for placeholders (`{entity:field}`)
3. **Parameter Matching:** IDE verifies enough substitution params provided
4. **Language Variants:** IDE generates entries for all project languages
5. **Compression:** Lexingine compresses message text to token stream
6. **Code Generation:** Compiler emits routine call with token stream
7. **Addressing:** Compiler resolves entity references in substitutions to
   ASM indirect addressing (same as MOV, LDV)

### Runtime Behavior

1. **Decompression:** Engine reads token stream, decompresses text
2. **Substitution:** Engine replaces placeholders with actual values
3. **Special Codes:** Engine interprets embedded codes (color, pause, sfx)
4. **Word-Wrap:** Engine wraps text to display width
5. **Queueing:** Engine queues message in text display buffer
6. **Display:** Engine renders message at next display update

### Message Template Syntax

Messages are authored in a template format. Max message length depends
on the target platform's buffer size (32-80 characters):

```
ID: msg_picked_up_item
EN: "You picked up the {item_torch:name}."
ES: "Recogiste el {item_torch:name}."
PT: "Você pegou o {item_torch:name}."

ID: msg_damage_taken
EN: "You take {value} damage![PAUSE:300]"
ES: "¡Recibes {value} daño!"
PT: "Você recebe {value} de dano!"

ID: msg_npc_speaks
EN: "[SFX:talk]{npc_guard:name}: {text}"
ES: "[SFX:talk]{npc_guard:name}: {text}"
```

**Placeholder Rules:**
- `{entity_id:field}` — Resolved at compile time by IDE to memory
  address + offset; runtime accesses the actual value
- Plain text — Compressed as-is via lexingine

**Special Code Format:**
- `[CODE:param]` — Embedded instruction for the engine
- `[PAUSE:milliseconds]` — Wait before continuing (all platforms)
- `[NEWLINE]` — Force line break (all platforms)
- `[COLOR:colorname]` — Set text color (platform-specific, optional)
- `[SFX:sound_id]` — Play sound (platform-specific, optional)

### Examples

**Simple static message:**
```
PRINT_MSG msg_sword_found
  — Displays: "You found a sword."
```

**Message with item name substitution:**
```
PRINT_MSG msg_picked_up_item, item_torch
  — Displays: "You picked up the wooden torch."
  — (item_torch:name resolved at compile time)
```

**Message with numeric value:**
```
DAMAGE 0x05
PRINT_MSG msg_damage_taken, 0x05
  — Displays: "You take 5 damage! [PAUSE]"
```

**Message with NPC and dynamic text:**
```
PRINT_MSG msg_npc_speaks, npc_guard, msg_greeting
  — Displays: "[SOUND] Guard: Hello, traveler!"
  — (npc_guard:name and msg_greeting resolved)
```

### Implementation Notes

**Lexingine Integration:**
- Message tokens are stored in the lexingine text bank
- The `PRINT_MSG` routine calls the lexingine decompressor
- Decompressor writes to a temporary text buffer
- Buffer size is platform-specific (32-80 characters depending on target)
- Compiler validates message length at compile time; oversized messages
  generate errors

**Language Selection (Compile-Time):**
- At compile time, the project selects a target language (e.g., `es_ES`)
- The compiler includes only that language's message variants
- If a message lacks the target language, the compiler uses EN fallback
- No runtime language switching; language is fixed per build

**Placeholder Resolution at Compile Time:**
- IDE scans `{entity_id:field}` placeholders in all messages
- IDE verifies entity and field exist
- IDE generates ASM variable name for the memory address
- Compiler inserts the indirect addressing reference into the routine

**Placeholder Substitution at Runtime:**
- Engine reads entity field value via ASM variable address indirection
- For numeric fields, engine converts value to ASCII
- Engine inserts converted value into decompressed text stream

**Display Queueing (Synchronous):**
- `PRINT_MSG` does not block; it queues the message for display
- Game is line-by-line synchronous; one message displayed per game tick
- Player input (or turn elapsed) triggers next message in queue
- Multiple `PRINT_MSG` calls in one event stack up for sequential display

**Platform-Specific Implementation:**
- Each target platform (Z80, 6502, 68K) has its own `PRINT_MSG` ASM
- Platform code handles character output, word-wrap, display width
- Optional codes (COLOR, SFX) are implemented only if platform supports
- Core codes (PAUSE, NEWLINE) work on all platforms

**Error Handling:**
- If `message_id` not found: return 1, display fallback error
- If decompression fails: return 1, skip message
- If placeholder mismatch: compile-time error (IDE validation)

### Open Questions (Resolved)

**Substitution Syntax:** `{entity_id:field}` is the standard syntax.
This is IDE-only; at compile time, the IDE resolves it to the actual
memory address + field offset. The runtime routine accesses the value
via ASM indirect addressing (same as MOV/LDV).

**Special Code Set:** Each target platform (Z80, 6502, 68K) has its own
ASM implementation of `PRINT_MSG`, adapted to platform capabilities.
Essential codes (PAUSE, newline handling) are implemented on all
platforms. Platform-specific codes (COLOR, SFX) gracefully degrade or
are omitted if the platform lacks capability. The IDE generates code
appropriate to the target platform.

**Text Buffer Size:** Buffer size is platform-specific, optimized for
each target's display width and available RAM:
- **ZX Spectrum, MSX:** 32 characters max
- **Amstrad CPC, C64:** 40 characters max
- **Atari ST:** 80 characters max
  This ensures efficient RAM usage and respects each platform's constraints.

**Fallback Strategy:** Language selection is compile-time only. If a
game compiles for a specific language (e.g., `es_ES`), the compiler
includes only that language's message variants. If a message lacks a
variant for the target language, the compiler uses EN (English) as the
fallback. This is enforced at build time; no runtime language switching.

**Async Display:** No async needed. The game is synchronous and
line-by-line. `PRINT_MSG` queues messages; they display one per game
tick or on player input. Blocking behavior simplifies implementation.

**Platform Variants:** Yes. Each core routine (including `PRINT_MSG`) has
a platform-specific ASM implementation for Z80, 6502, and 68K. The
compiler selects the right version based on the target platform.
Constraints (registers, memory, display width) are handled by the
platform-specific code.