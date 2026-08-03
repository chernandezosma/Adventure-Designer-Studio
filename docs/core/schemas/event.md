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

# Events
An event is a named signal (<uint8_t>) emitted by the engine when a specific 
situation occurs during play — for example, the main character enters 
a scene and, at that moment, the light goes out, leaving the scene 
completely dark, or a NPC inflicts damage to main character, that is an
_on_damage_ event which tell how many damage do.

Events are the connective tissue between *what happens* in the game world
and *how the world reacts*. They are not actions themselves: an event
merely announces that a situation has occurred. The reaction is defined
separately, by the triggers attached to game elements.

Any game element that supports triggers can declare which events it will
listen to, by adding to the item itself. This elements could be scenes, 
items, containers, NPCs, and the player character itself as dispatcher and 
as listener. Elements reference events by `EventId` (unit8_t) rather than 
embedding effects inline, which would require programming knowledge; when 
the corresponding event name will be fired for an element, the engine 
executes the referenced event's.

This document defines how events are declared as part of each game element
that supports them, and establishes the naming and uniqueness rules that
keep event handling predictable at runtime.

## Schema
This is the schema which define an event defined in the game

```json
{
  "event": [
    {
      "id": "<uint8_t>",
      "descriptions": "<Descriptions>",
      "target": {
        "id": "<Object ID>",
        "type": "<uint8_t>"
      },
      "delay": "<uint8_t> — turns to wait before executing; 0 means immediate",
      "action": "<uint8_t>"
    }
  ]
}
```

### Details

<font color="#C27AFF">id</font>:  Unique `uint8_t` identifier for the event 
(an `EventId`, the same phantom-typed id as `SceneId`/`ObjectId` — see 
[Id implementation](common-structures.md#id-implementation)). This is what other schemas — scene `triggers`, 
character `state.effect` — reference.

<font color="#C27AFF">descriptions</font>: These are the event's 
description. See the [Texts definition](common-structures.md#descriptions-definition) 

<font color="#C27AFF">target</font>: This is an `<uint8_t>` as bitmap 
which will identified the type of event, the possible values are:

```
    7   6   5   4   3   2   1   0
    --------------------------------
    |   |   |   |   |   |   |   |
    |   |   |   |   |   |   |   |---> Global - Event set globally for the game
    |   |   |   |   |   |   |-------> Scene - Event relative to the scene; on-enter, on-exit.
    |   |   |   |   |   |-----------> Item - Event relative to the item; on-damage.
    |   |   |   |   |---------------> Player - Event relative to the player; on-dead, on-heal
    |   |   |   |-------------------> (Reserved)
    |   |   |-----------------------> (Reserved)
    |   |---------------------------> (Reserved)
    |-------------------------------> (Reserved)
```

<font color="#C27AFF">delay</font>: Number of game turns to wait before 
executing the action. `0` means immediate execution in the same turn the 
event fires.

<font color="#C27AFF">action</font>: The ordered list of routine calls that 
implement the event's actual behaviour. Each entry's `calls-stack` names 
a Tier 1/Tier 2 core routine (see [core-routines.md](core-routines.md)) or an 
author-defined routine composed from them; `parameters` supplies that 
routine's arguments. This replaces the older `target`/`command`/`delay`/
`parameters` effect shape — routines are called directly instead of going 
through a fixed condact table.

## Declaring triggers on an element

Any element that supports triggers (scenes, items, characters, ...)
references events by `EventId` rather than embedding effects inline. A
scene's `triggers` field, for example, maps a global scene event name
(see [scene.md → Global events](scene.md#global-triggers)) to an array of
`EventId`s:

```json
{
  "triggers": {
    "<triggerId>": ["<EventId>", "<EventId>", "<EventId>", "<EventId>"],
    "on_turn": ["<EventId>", "<EventId>"]
  }
}
```

Every single entry in _triggers_ list are a valid `EventId` declared and 
registered. The list could be one or several events. In case to be more than
one, they will be executed in the same order that they are declared. 

Each `EventId` must resolve to an entry already defined in the Event
Schema above. A character's single active state follows the same
pattern via its `effect` field — see
[character.md → Effects](common-structures.md#effects-damages--heal)

## Implementation

Events compile to a flat lookup table, one entry per authored event:

```
[ event_id | code_ptr ]
  uint8_t    uint16_t     = 3 bytes per entry
```

`code_ptr` addresses the compiled `code` calls-stack for that event.
Compiling `code` itself — resolving each `calls-stack` entry to a routine
address and packing its `parameters` — follows the calling convention
already defined in [core-routines.md](core-routines.md); this document
does not redefine a separate dispatch mechanism for routine calls.

Element-level references (`triggers` on a scene, `effect` on a character
state) compile to a second small table resolving `(owner_type, owner_id,
event_name)` to the `EventId` it points at:

```
[ owner_type | owner_id | event_name_id | event_id ]
  uint8_t      uint8_t    uint8_t          uint8_t    = 4 bytes per entry
```

At runtime, firing an event for a given owner and event name is therefore
a single lookup into this table followed by a direct jump to that event's
`code_ptr` — no per-owner effect list or command-id jump table is
required.