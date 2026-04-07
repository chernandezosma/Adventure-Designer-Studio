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

# Scene

A scene represents a location in the game world — what classic text adventures call a *room*. It is the
primary container for items and the hub through which the player navigates. A scene has no mass, no
vocabulary entry of its own (the lexicon engine manages all name resolution), and no combinatorial
behaviour. Its character comes entirely from the items it holds, the exits it exposes, and the triggers
it defines.

## Scene definition

```json
{
  "scenes": [
    {
      "DAAD": {
        "comment": "DAAD section to keep backward compatibility",
        "locno": "<valid DAAD location number>"
      },
      "id": "<RoomId — uint8_t via Id<RoomTag>>",
      "name": "<String> | max: 128",
      "descriptions": {
        "description": "<String> | max: 128  — shown on revisit or LOOK",
        "long_description": "<String> | max: 255 — shown on first visit",
        "odor_description": "<String> | max: 255 — optional; omit if no ambient smell",
        "sound_description": "<String> | max: 255 — optional; omit if no ambient sound"
      },
      "image": "<String> — filename or base64-encoded image",
      "affordances": "<uint8_t> — see affordance bitmap below",
      "state": "<uint8_t> — see state bitmap below",
      "exits": {
        "comment": "Each direction holds a RoomId or null if no exit exists",
        "north":     "<RoomId | null>",
        "south":     "<RoomId | null>",
        "east":      "<RoomId | null>",
        "west":      "<RoomId | null>",
        "northeast": "<RoomId | null>",
        "northwest": "<RoomId | null>",
        "southeast": "<RoomId | null>",
        "southwest": "<RoomId | null>",
        "up":        "<RoomId | null>",
        "down":      "<RoomId | null>"
      },
      "items": "<uint16_t> — pointer to list of ObjectIds present in this scene at load time",
      "triggers": [
        {
          "on_enter": [
            {
              "target": { "type": "<room | item | player | global>", "id": "<RoomId | ObjectId | null>" },
              "command": "<String> — e.g. set_state, move_item, fire_event, swap_description",
              "delay": "<uint8_t> — turns to wait before executing; 0 means immediate",
              "parameters": {
                "comment": "Variable key-value pairs; content depends on the command type",
                "key": "value"
              }
            }
          ]
        },
        {
          "on_turn": [
            {
              "target": { "type": "<room | item | player | global>", "id": "<RoomId | ObjectId | null>" },
              "command": "<String>",
              "delay": "<uint8_t>",
              "parameters": { "key": "value" }
            }
          ]
        }
      ]
    }
  ]
}
```

### Details

- **DAAD**: Backward-compatibility block. `locno` maps this scene to the equivalent DAAD location number
  for projects that need to interoperate with or migrate from DAAD source files.

- **id**: Unique identifier using the phantom-typed `Id<RoomTag>` template, consistent with `ObjectId`
  and `EventId`. See [Id implementation](#id-implementation).

- **name**: Human-readable scene name shown in the IDE and in-game where appropriate. Max 128 bytes.

- **descriptions**: Group of text fields describing the scene from different sensory perspectives.
    - `description` — brief text shown on revisit or when the player types LOOK.
    - `long_description` — detailed text shown on first visit.
    - `odor_description` — optional. If present, the engine responds to SMELL in this scene.
      No affordance bit is needed; field presence implies the capability.
    - `sound_description` — optional. If present, the engine responds to LISTEN in this scene.
      Same rule applies.

- **image**: Filename or base64-encoded image used by the IDE and optionally rendered in-game
  on platforms that support graphics.

- **affordances**: Static `uint8_t` bitmap declared by the author at design time. Never modified
  at runtime. See [Affordance bitmap](#affordance-bitmap).

- **state**: Dynamic `uint8_t` bitmap written by the engine during play. Zero-initialised at load time.
  See [State bitmap](#state-bitmap).

- **exits**: Fixed set of 10 named directions. Each holds a `RoomId` referencing the destination
  scene, or `null` if no passage exists in that direction. Conditional exits (locked doors, barriers)
  are modelled as items placed at the exit, not as properties of the exit itself.

- **items**: Pointer to the flat list of `ObjectId`s present in this scene when the game loads.
  Item placement is owned by the scene; items do not carry their own location field.

- **triggers**: Array of event handlers. The engine defines a fixed set of global events; each scene
  declares handlers only for the events it needs to respond to. Each entry is a single-key object
  where the key is the event name and the value is an array of effects. The same event key must not
  appear more than once in the array. See [Triggers](#triggers).

---

## Affordance bitmap

Affordances are static properties of the scene, set by the author and never changed at runtime.

```
    7   6   5   4   3   2   1   0
    --------------------------------
    |   |   |   |   |   |   |   |
    |   |   |   |   |   |   |   |--> Dark      — no ambient light; player needs a light source
    |   |   |   |   |   |   |-------> Lightable — darkness can be resolved by the player
    |   |   |   |   |   |-----------> Visitable — scene is known/reachable in principle
    |   |   |   |   |--------------> (Reserved)
    |   |   |   |------------------> (Reserved)
    |   |   |---------------------> (User defined 1)
    |   |-------------------------> (User defined 2)
    |-----------------------------> (User defined 3)
```

### Affordance notes

- **Dark**: The scene has no ambient light. The engine will block normal descriptions and actions
  unless a light source is present in the scene or carried by the player.

- **Lightable**: The scene contains a means by which the player can introduce light (a fireplace,
  a torch bracket, a switch). This affordance signals to the author and the IDE that a light-source
  item should be placed here; it does not itself provide light.

- **Visitable**: The scene exists in the game world and may be referenced or seen (through a window,
  mentioned in dialogue, visible on a map) but is currently unreachable by normal navigation.
  The engine may lift this restriction via a trigger.

---

## State bitmap

State bits are dynamic. All bits are zero at load time. The engine and trigger system write them
during play; the author never sets them directly in the authoring schema.

```
    7   6   5   4   3   2   1   0
    --------------------------------
    |   |   |   |   |   |   |   |
    |   |   |   |   |   |   |   |---> Visited — set by engine on first player entry
    |   |   |   |   |   |   |-------> Lit     — set by engine when a light source is active
    |   |   |   |   |   |-----------> (Reserved)  
    |   |   |   |   |----------\       
    |   |   |   |--------------->---> Environmental condition (3-bit packed value, bits 4–6)                     see condition table below
    |   |   |   |--------------/
    |   |   |   |-------------------> (Reserved)
```

### Environmental condition (bits 4–6)

These three bits encode the current hazardous condition of the scene as a packed value.
The condition is set to `000` (None) at load time and changed by a trigger firing an effect
on `on_enter` or any other relevant event.

```
    Value   Condition
    -----   ---------
    000     None        — scene is safe
    001     Flooded     — scene is submerged or filling with water
    010     Burning     — scene is on fire
    011     Poisonous   — scene contains toxic gas or substance
    100     Freezing    — scene is dangerously cold
    101     (Reserved)
    110     (User defined 1)
    111     (User defined 2)
```

The engine reads these bits each turn while the player is in the scene and applies the
corresponding consequences (damage, movement restriction, item destruction, etc.).

---

## Triggers

Triggers connect globally defined engine events to local scene effects. The engine defines
the event vocabulary; each scene implements only the handlers it needs. An event with no
handler in a given scene is silently ignored.

### Constraints

- Each event key must appear **at most once** in the triggers array. Multiple effects for the
  same event are expressed as multiple entries inside that event's effects array, not as
  duplicate keys.
- Effects within an event are executed in array order.

### Global events

| Event             | Fires when...                                       |
|-------------------|-----------------------------------------------------|
| `on_enter`        | Player arrives in the scene                         |
| `on_exit`         | Player leaves the scene                             |
| `on_examine`      | Player examines the scene (LOOK / EXAMINE)          |
| `on_turn`         | Each game turn while the player is in the scene     |
| `on_item_taken`   | An item is picked up from this scene                |
| `on_item_dropped` | An item is dropped into this scene                  |
| `on_item_used`    | An item is used while in this scene                 |

### Effect structure

Each event maps to an array of effects. Every effect answers three questions:
what is affected (`target`), what changes (`command`), and to what value (`parameters`).
The `target.type` field acts as the discriminator — exactly one target category applies
per effect.

```json
"triggers": [
  {
    "on_enter": [
      {
        "target": { "type": "room", "id": "3" },
        "command": "set_state",
        "delay": 0,
        "parameters": { "condition": "burning" }
      },
      {
        "target": { "type": "player", "id": null },
        "command": "apply_damage",
        "delay": 3,
        "parameters": { "amount": "10" }
      }
    ]
  },
  {
    "on_turn": [
      {
        "target": { "type": "item", "id": "12" },
        "command": "move_item",
        "delay": 0,
        "parameters": { "destination": "5" }
      }
    ]
  }
]
```

- **target.type**: The category of entity being affected — `room`, `item`, `player`, or `global`.
- **target.id**: The specific entity by `RoomId` or `ObjectId`. `null` when type is `player` or `global`.
- **command**: The operation to perform. The command vocabulary is **closed and engine-defined** —
  a fixed set of predefined condacts that the author combines declaratively. No author-written code
  is ever required. The compiler validates every command name against the known condact table and
  rejects anything unrecognised. See [Condacts](#condacts).
- **delay**: Number of game turns to wait before executing the effect. `0` means immediate execution
  in the same turn the event fires. Stored as `uint8_t` — range 0 to 255 turns. Time advances only
  when the player acts, making turns the only meaningful unit on all target platforms.
- **parameters**: A variable key-value map whose valid keys depend on the command. The layout is
  fixed and documented per condact — the runtime handler always knows the exact byte layout of
  its own parameter data.

---

## Implementation

### Id implementation

Scene identifiers follow the same phantom-typed pattern used across all ADS entities:

```c++
template<typename Tag>
struct Id {
    uint8_t value;
    explicit Id(uint8_t v) : value(v) {}
    bool operator==(const Id&) const = default;
};

struct RoomTag   {};
struct ObjectTag {};
struct EventTag  {};

using RoomId   = Id<RoomTag>;
using ObjectId = Id<ObjectTag>;
using EventId  = Id<EventTag>;
```

`RoomId` values are stable at design time and assigned by the IDE. The compiler may reorder
them for output optimisation but preserves all cross-references.

### Compiled representation

At compile time the authoring JSON is reduced to flat, packed structures suitable for the target
platform. All data lives in RAM — the target platforms have no separate ROM segment. The
`affordances` byte is logically read-only (never written at runtime); the `state` byte is
mutable and zero-initialised at startup.

The descriptions are tokenised by the lexicon engine and stored as token ID arrays; no ASCII
strings appear in the compiled output.

The `exits` block compiles to a fixed 10-entry array of `RoomId` values (one per direction),
with a sentinel value (`0xFF`) indicating no exit. This keeps the structure a known size
regardless of how many exits the scene actually has.

### Trigger runtime representation

The authoring trigger structure is transformed by the compiler into three flat memory regions:

**Event dispatch table** — one entry per (event, scene) pair, sorted by event then scene:

```
[ event_id | scene_id | effect_list_ptr ]
  uint8_t    uint8_t    uint16_t           = 4 bytes per entry
  0xFF sentinel marks end of table
```

**Effect lists** — one list per (event, scene) pair, each effect is a fixed 6-byte record:

```
[ command_id | target_type | target_id | delay | param_lo | param_hi ]
  uint8_t      uint8_t       uint8_t     uint8_t  uint8_t    uint8_t
  0xFF sentinel marks end of list
```

Parameters are inlined as two bytes directly in the effect record — no pointer indirection.
Each condact handler interprets those two bytes according to its own fixed layout.

**Deferred effect queue** — static fixed-size array in RAM, zero-initialised at load time:

```
[ turns_remaining | command_id | target_type | target_id | param_lo | param_hi ]
  uint8_t           uint8_t      uint8_t       uint8_t     uint8_t    uint8_t
```

8 slots × 6 bytes = 48 bytes of RAM reserved unconditionally. Each turn the engine
decrements all active slot counters and executes any that reach zero.

**Concrete example** — the trigger sample from the authoring schema above compiles to:

```
Event dispatch table:
    01 05 00 01     on_enter / scene 5 / effects at 0x0100
    02 05 0D 01     on_turn  / scene 5 / effects at 0x010D
    FF              end of table

Effect list at 0x0100 (on_enter):
    01 01 03 00 02 00   set_state   / room 3   / delay 0 / condition=burning
    02 02 FF 03 0A 00   apply_damage/ player   / delay 3 / amount=10
    FF                  end of list

Effect list at 0x010D (on_turn):
    03 03 0C 00 05 00   move_item   / item 12  / delay 0 / destination=5
    FF                  end of list
```

Total for this example: 9 bytes dispatch table + 20 bytes effect lists = **29 bytes**.

### Runtime dispatch

The engine dispatches condact execution via a **jump table** indexed by `command_id`. The
table is fixed at engine build time — its entries never change for a given ADS release:

```
jump_table:
    dw handle_nop           ; 0x00
    dw handle_set_state     ; 0x01
    dw handle_apply_damage  ; 0x02
    dw handle_move_item     ; 0x03
    dw handle_destroy_item  ; 0x04
    dw handle_swap_desc     ; 0x05
    dw handle_print_message ; 0x06
    dw handle_end_game      ; 0x07
    ...
```

No string comparison ever occurs at runtime. The compiler resolves all command name strings
to their `command_id` bytes at compile time and rejects any unrecognised command.

---

## Condacts

Condacts are the closed, engine-defined vocabulary of commands available to the author in
trigger effect arrays. The author never writes code — they combine condacts declaratively.
The full condact set grows as subsystems are designed; the table below reflects the current
baseline for scene-level triggers.

| command_id | Name              | Target        | Parameters              | Effect                                          |
|------------|-------------------|---------------|-------------------------|-------------------------------------------------|
| 0x00       | `nop`             | any           | —                       | No operation; placeholder                       |
| 0x01       | `set_state`       | room          | condition: uint8_t      | Set environmental condition on target room      |
| 0x02       | `apply_damage`    | player        | amount: uint8_t         | Reduce player health by amount                  |
| 0x03       | `move_item`       | item          | destination: uint8_t    | Move item to destination room                   |
| 0x04       | `destroy_item`    | item          | —                       | Remove item from the game entirely              |
| 0x05       | `swap_description`| room          | token_ptr: uint16_t     | Replace active description with alternate text  |
| 0x06       | `print_message`   | global        | token_ptr: uint16_t     | Display a message to the player                 |
| 0x07       | `end_game`        | global        | outcome: uint8_t        | Trigger win (0x01) or lose (0x00) condition     |

This table will be expanded as the player, inventory, and other subsystems are defined.
Each new subsystem contributes its own condact entries with assigned `command_id` values.