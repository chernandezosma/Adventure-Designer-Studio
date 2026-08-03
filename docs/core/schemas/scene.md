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

A scene represents a location in the game world — what classic text adventures
call a *scene*. It is the primary container for items and the hub through which
the player navigates. A scene has no mass, no vocabulary entry of its own (the
lexicon engine manages all name resolution), and no combinatorial behaviour. Its
character comes entirely from the items it holds, the exits it exposes, and the
triggers it defines.

## Scene definition

```json
{
  "scenes": [
    {
      "id": "<SceneId — uint8_t via Id<SceneTag>>",
      "name": "<Text, 128>",
      "descrptions": "<Descriptions> — see common-structures.md#decriptions-definition",
      "image": "<String> — filename or base64-encoded image",
      "affordances": "<uint8_t> — see affordance bitmap below",
      "flags": "<uint8_t> — see Flags bitmap below",
      "state": "<uint8_t> — id into the States catalog below",
      "exits": {
        "comment": "Each direction holds a SceneId or null if no exit exists",
        "north": "<SceneId | null>",
        "south": "<SceneId | null>",
        "east": "<SceneId | null>",
        "west": "<SceneId | null>",
        "northeast": "<SceneId | null>",
        "northwest": "<SceneId | null>",
        "southeast": "<SceneId | null>",
        "southwest": "<SceneId | null>",
        "up": "<SceneId | null>",
        "down": "<SceneId | null>"
      },
      "items": "<uint16_t> — pointer to list of ObjectIds present in this scene at load time",
      "triggers": "<Triggers> — see events.md#declaring-triggers-on-an-element"
    }
  ]
}
```

### Details

<font color="#C27AFF">id</font>: Unique identifier using the phantom-typed
`Id<SceneTag>` template, consistent with `ObjectId` and `EventId`.
See [Id implementation](#id-implementation).

<font color="#C27AFF">name</font>: This is the human readable item's name. It
will show in the game as the item's name. It will be treated as a string with a
max length of 128 bytes, but internally will be stored as <uint32_t>, and it
represents a Lexicon id.

<font color="#C27AFF">Descriptions</font>: Group of text fields describing the
scene from different sensory perspectives. See the
shared [Texts definition](common-structures.md#decriptions-definition) in
`common-structures.md`.

<font color="#C27AFF">image</font>: This field will be a filename or
base64-encoded image used in the IDE and optionally.

<font color="#C27AFF">affordances</font>: Static `uint8_t` bitmap declared by
the author at design time. Never modified at runtime.
See [Affordance bitmap](#affordance-bitmap).

<font color="#C27AFF">flags</font>: Dynamic `uint8_t` bitmap written by the
engine during play. Zero-initialised at load time. Independent boolean flags
that may be set simultaneously alongside any active `state`.
See [Flags bitmap](#flags-bitmap).

<font color="#C27AFF">state</font>: This is the scene condition, `<uint8_t>`
that holds the `id`
of the scene's single currently-active environmental condition, see
[Environmental States](#environmental-states).

<font color="#C27AFF">exits</font>: Fixed set of 10 named directions. Each holds
a `SceneId`
referencing the destination scene, or `null` if no passage exists in that
direction. Conditional exits (locked doors, barriers) are modeled as items
placed at the exit, not as properties of the exit itself.

<font color="#C27AFF">items</font>: Pointer to the flat list of Items'
(`ObjectId`) present in the scene when the player get in or game loads it. Item
placement is owned, and responsability, of the scene; items do not define their
location's field.

<font color="#C27AFF">triggers</font>: Map from global scene event name to an
array of `EventId`s referencing entries in [Event Schema](event.md#schema). The
engine defines a fixed set of global events; each scene declares listeners only
for the events it needs to respond to. The same event key must not appear more
than once. See [Triggers](#triggers).

<font color="#008000">backguard_compatibility</font>: In the future we will keep
a section in each item to store the configurations from the legacy systems which
affect to current one. it is only present when any feature part of the legacy
system is taken to the current system. For instance, in **DAAD** the scene ID is
named `locno` and it could be diffent than ADS `SceneId` `locno`, so this will
be mapped using this section. Of course, not only DAAD will have a place in this
section, every single interpreter that the importer support will have its own
section.
---

## Affordance bitmap

Affordances are static properties of the scene, set by the author and never
changed at runtime.

```
    7   6   5   4   3   2   1   0
    --------------------------------
    |   |   |   |   |   |   |   |
    |   |   |   |   |   |   |   |---> Dark      — no ambient light; player needs a light source.
    |   |   |   |   |   |   |-------> Lightable — darkness can be resolved by the player.
    |   |   |   |   |   |-----------> Visitable — scene is known/reachable. 
    |   |   |   |   |---------------> (Reserved)
    |   |   |   |-------------------> (Reserved)
    |   |   |-----------------------> (Reserved)
    |   |---------------------------> (User defined 1)
    |-------------------------------> (User defined 2)
```

### Affordance notes

<font color="#C27AFF">Dark</font>: The scene has no ambient light. The engine
will block normal descriptions and actions unless a light source is present in
the scene or carried by the player.

<font color="#C27AFF">Lightable</font>: The scene contains a means by which the
player can introduce light (a fireplace, a torch bracket, a switch). This
affordance signals to the author and the IDE that a light-source item should be
placed here; it does not itself provide light.

<font color="#C27AFF">Visitable</font>: The scene exists in the game world and
may be referenced or seen (through a window, mentioned in dialogue, visible on a
map) but is currently unreachable by normal navigation. The engine may lift this
restriction via a trigger.

---

## Flags bitmap

Flag bits are dynamic. All bits are zero at load time. The engine writes them
during play; the author never sets them directly in the authoring schema. Unlike
`state` below, flags are independent booleans — any combination may be active
simultaneously (a scene can be Visited and Lit at the same time, regardless of
its current `state`).

```
    7   6   5   4   3   2   1   0
    --------------------------------
    |   |   |   |   |   |   |   |
    |   |   |   |   |   |   |   |---> Visited   — set by engine on first player entry
    |   |   |   |   |   |   |-------> Lit       — set by engine when a light source is active
    |   |   |   |   |   |-----------> Open      - set by engine when the room is open
    |   |   |   |   |---------------> (Reserved)
    |   |   |   |-------------------> (Reserved)
    |   |   |-----------------------> (Reserved)
    |   |---------------------------> (User defined 1)
    |-------------------------------> (User defined 2)
```

The Flags could be grow by adding some ones.

---

## Environmental States

Before continuing with this section, we need to have a look to the base info for
the [State Catalog Pattern](common-structures.md#state-catalog-pattern). Scenes
use that pattern to define their own catalog of environmental conditions (dark,
flooded, collapsing, etc.). After that we need to know that in this case, the
first position (0) is reserved for `None` to indicate that the scene has no
remarkable state, so to speak, is a normal scene.
---

## Triggers

Triggers connect globally defined engine events to local scene effects. The
engine defines the event vocabulary; each scene implements only the handlers it
needs. An event with no handler in a given scene is silently ignored.

### Constraints

- Each event key must appear **at most once** in `triggers`. Multiple
  `EventId`s for the same event name go in that key's array, not as duplicate
  keys.
- `EventId`s within an event name are fired in array order.

### Global triggers

These triggers are referenced by an <uint8_t> and internally, at IDE, we will
use that value to match which will fire the event.

| id   | Trigger           | Fires when                                      |
|------|-------------------|-------------------------------------------------|
| 0x01 | `on_enter`        | Player arrives in the scene                     |
| 0x02 | `on_exit`         | Player leaves the scene                         |
| 0x03 | `on_examine`      | Player examines the scene (LOOK / EXAMINE)      |
| 0x04 | `on_turn`         | Each game turn while the player is in the scene |
| 0x05 | `on_item_taken`   | An item is picked up from this scene            |
| 0x06 | `on_item_dropped` | An item is dropped into this scene              |
| 0x07 | `on_item_used`    | An item is used while in this scene             |

### Declaring handlers

`triggers` does not embed effects inline — it maps a global event name to an
array of `EventId`s, each referencing an entry already defined in
[Event Schema](event.md#schema):