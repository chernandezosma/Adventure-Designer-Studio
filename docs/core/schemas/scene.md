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
LexEngine manages all name resolution), and no combinatorial behaviour. Its
character comes entirely from the items it holds, the exits it exposes, and the
triggers it defines.

## Scene definition

```json
{
  "scenes": [
    {
      "id": "<SceneId — uint8_t via Id<SceneTag>>",
      "name": "<String, 128>",
      "descriptions": "<Descriptions> — see common-structures.md#decriptions-definition",
      "image": "<String> — filename or base64-encoded image",
      "affordance": "<affordance[]> — see Affordances definitions below",
      "flags": "<uint8_t> — see Flags bitmap below",
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

<font color="#C27AFF">name</font>: The scene's human-readable name — shown as the
scene's label in the IDE and anywhere the engine needs to name the scene
itself. A literal `<String>` with a maximum length of 128 bytes — unlike
`item.name` or `character.name`, it is not a LexEngine entry: the scene name
is not player-input vocabulary (the player never types it to navigate), so it
does not go through the LexEngine's NLP/vocabulary pipeline. Whether it is
translated per language is entirely the author's decision, made outside this
schema (e.g. by compiling a different `name` per target language build), the
same treatment as [game.title](game.md#details).

<font color="#C27AFF">Descriptions</font>: Group of text fields describing the
scene from different sensory perspectives. See the
shared [Texts definition](common-structures.md#decriptions-definition) in
`common-structures.md`.

<font color="#C27AFF">image</font>: This field will be a filename or
base64-encoded image used in the IDE and optionally.

<font color="#C27AFF">affordance</font>: An array of `<affordance>` entries —
see [Affordances definitions](#affordances-definitions) below, and
item.md's identical `affordance` field, which this mirrors. Set by the
author at design time. Unrelated to `triggers` below, which resolves
fixed engine-defined event keys to `EventId`s, not author-named triggers.

<font color="#C27AFF">flags</font>: Dynamic `uint8_t` bitmap written by the
engine during play. Zero-initialised at load time. Independent boolean flags
that may be set simultaneously.
See [Flags bitmap](#flags-bitmap).

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

## Affordances definitions

Same shape as item.md's `affordance` field: an array pairing an
author-chosen name with the trigger names it fires.

```json
{
  "affordance": [
    {
      "name": "<string:128>",
      "trigger": [
        "on_<name>",
        "on_<name>"
      ]
    }
  ]
}
```

`name` is free text, not a fixed enum. Concepts formerly modeled as scene
affordance bits — e.g. "Dark" (no ambient light; the player needs a light
source), "Lightable" (the scene holds a means to introduce light), or
"Visitable" (known/reachable but not currently accessible by normal
navigation) — are examples of names an author may use here, resolved via
whatever trigger names they attach. The author is responsible for defining
those trigger names and the logic that later resolves them.

---

## Flags bitmap

Flag bits are dynamic. All bits are zero at load time. The engine writes them
during play; the author never sets them directly in the authoring schema. Flags
are independent booleans — any combination may be active simultaneously (a
scene can be Visited and Lit at the same time).

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

See the [Global Triggers](common-structures.md#global-triggers) There are defined 
all triggers and its corresponding to each entity. 

### Declaring handlers

`triggers` does not embed effects inline — it maps a global event name to an
array of `EventId`s, each referencing an entry already defined in
[Event Schema](event.md#schema):