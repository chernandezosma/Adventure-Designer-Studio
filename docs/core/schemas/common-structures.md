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

# Common Structures

A scene represents a location in the game world — what classic text adventures
call a *scene*. It is the primary container for items and the hub through which
the player navigates. A scene has no mass, no vocabulary entry of its own (the
LexEngine manages all name resolution), and no combinatorial behaviour. Its
character comes entirely from the items it holds, the exits it exposes, and the
triggers it defines.

Index

- [Id implementation](#id-implementation)
- [Texts](#decriptions-definition)
- [State Catalog Pattern](#state-catalog-pattern)
- [Damages](#Damages)

--- 

## Preface notes

In general all Strings are not stored as a literal in this schema. Like any
other player-facing text, it must be authored through the
[LexEngine](../lexengine/vocabulary.md) so a translated form exists for every
language the project authors in — not just the one language a given build is
compiled for.
`name` therefore holds a LexEngine entry `id` (`uint32_t`, per the
[Canonical Node Structure](../lexengine/vocabulary.md#canonical-node-structure)),
and the compiler resolves it to that build's target-language text at compile
time, exactly as it does for the
`texts` group
in [common-structures.md](common-structures.md#decriptions-definition).

## Id implementation

All identifiers identified by Id<entity> follow the same phantom-typed pattern
used across all ADS entities:

```c++
template<typename Tag>
struct Id {
    uint8_t value;
    explicit Id(uint8_t v) : value(v) {}
    bool operator==(const Id&) const = default;
};

struct SceneTag   {};
struct ObjectTag {};
struct EventTag  {};

using SceneId   = Id<SceneTag>;
using ObjectId = Id<ObjectTag>;
using EventId  = Id<EventTag>;
```

`SceneId` values are stable at design time and assigned by the IDE. The compiler
may reorder them for output optimisation but preserves all cross-references.

### Compiled representation

At compile time the authoring JSON is reduced to flat, packed structures
suitable for the target platform. All data lives in RAM — the target platforms
have no separate ROM segment. The `affordances` byte is logically read-only
(never written at runtime); the `flags` byte and the
`state` byte are both mutable and zero-initialised at startup — `flags`
as independent booleans, `state` as a single catalog `id` (`0` = `None`).

The descriptions are tokenised by the LexEngine and stored as token ID
arrays; no ASCII strings appear in the compiled output.

The `exits` block compiles to a fixed 10-entry array of `SceneId` values (one
per direction), with a sentinel value (`0xFF`) indicating no exit. This keeps
the structure a known size regardless of how many exits the scene actually has.

How `triggers` compiles and dispatches at runtime — the event lookup table, the
`EventId` resolution, and how a fired event's `code` executes — is documented
in [events.md → Implementation](events.md#implementation), since that mechanism
is shared by every element that supports triggers, not just scenes.

## Descriptions definition

A field type of `<Text>` holds a LexEngine entry `id` (`uint32_t`, per
the [Canonical Node Structure](../lexengine/vocabulary.md#canonical-node-structure)),
and the compiler will resolves it to that build's target-language text at
compile time.

The size/length limits and `<String>` markers below describe the *authored* text
the LexEngine entry ultimately resolves to, not the field's storage type.

So the definition for a text, that we will use in the rest of these documents,
it is defined as below and we can use as `<Text,128>` to indicate a Text with a
128 bytes of maximum length:

```json
{
  "Text": {
    "<uint32_t>": "LexEngine Id (pointer)",
    "<uint8_t>": "Maximum string length"
  }
}
```

None of the four fields below are stored as literal strings in this schema. Like
any other player-facing text, each must be authored through
the [LexEngine](../lexengine/vocabulary.md) so a translated form exists for every
language the project authors in — not just the one language a given build is
compiled for.

```json
{
  "desriptions": {
    "normal": "<Text> — LexEngine id; text max: 128 — shown on revisit or LOOK",
    "long": "<Text> — LexEngine id; text max: 255 — shown on first visit",
    "odor": "<Text> —Preface notes LexEngine id; text max: 255 — optional; omit if no ambient smell",
    "sound": "<Text> — LexEngine id; text max: 255 — optional; omit if no ambient sound"
  }
}
```

### Details

Each of the entries above is a LexEngine-backed text field describing some sensory
or narrative aspect of the object holding this `texts` group (scene, item, or
character). They are detailed below.

If `odor` or `sound` are present, the engine will responds to `SMELL` or
`LISTEN` vocabulary verbs when the user use them in the scene. Those text are
not shown by default, except the flags `SHOW_ODOR_TEXT` or
`SHOW_LISTENINT_TEXT` are set. (See
scene [flags](scene.md#decriptions-definition))

<font color="#C27AFF">normal</font>: Short text shown whenever the object is
revisited or the player issues a `LOOK` (or equivalent) command. Resolves to a
string with a max length of 128 bytes and should always be present.

<font color="#C27AFF">long</font>: Extended text shown the first time the player
encounters the object, for instance at the `EXAMINE` command. Resolves to a
string with a max length of 255 bytes and should always be present.

<font color="#C27AFF">odor</font>: Text describing the object's ambient smell,
used when the player explicitly smells it or when a `SMELL`-style command
surfaces sensory detail. Resolves to a string with a max length of 255 bytes;
this field is optional and should be omitted entirely if the object has no
distinguishable odor.

<font color="#C27AFF">sound</font>: Text describing the object's ambient sound,
used when the player explicitly listens (`LISTEN`) or when sensory detail is
surfaced. Resolves to a string with a max length of 255 bytes; this field is
optional and should be omitted entirely if the object is silent.

## States

This is the common pattern behind every state field in the schemas (e.g.
character's `condition`, scene's `state`, item's `state`): an entity can only
have one active catalog entry at a time, and considering that
<uint8_t> is an 8 bit type, it will give to us 255 entries, where 0 will be the
baseline/unafflicted `None` value. The entries

Entries 1 to 50 are reserved for System purposes, and in the upper positions of
the table, from 200 to 254 will be user-defined entries.

Each entity that uses this pattern defines its own concrete catalog following
this shape
(see [character.md → Conditions](character.md), [scene.md → Environmental States](scene.md#environmental-states),
and [item.md → States](item.md#states) for the domain-specific tables). Item's
catalog is the one exception to the shape below: its entries carry
`effectivity` instead of `effect: EventId` — see
[item.md → States](item.md#states) for details. The definition for each catalog
entry will be the following one, they cannot be editable, if you need to define
a new one you must define it above the 200 position:

```json
{
  "condition": [
    {
      "id": "<uint8_t>",
      "name": "<uint32_t> — LexEngine entry id; see ../lexengine/vocabulary.md",
      "category": "<uint8_t>",
      "description": "<Text, 128> — shown to the player while active",
      "effect": "<EventId> — fired when this condition becomes active."
    }
  ]
}
```

### Details

<font color="#C27AFF">id</font>: Unique `uint8_t` identifier for the condition,
referenced by a character's `condition` field. `0` is reserved for `None`.

<font color="#C27AFF">name</font>: `uint32_t` reference to a
[LexEngine entry](../lexengine/vocabulary.md#lexengine-entry-structure) rather than a
literal string, so the condition's visible name is translated into every
authored language. Used to identify the condition in the IDE and in
triggers/conditions; the tables below show its English form for readability
only.

<font color="#C27AFF">category</font>: Is the group where the condition is. It
is a bitmap with the category definition.
See [Categries](character.md#categories).

<font color="#C27AFF">description</font>: Condition description to shown to
reflect it and its changes. The string will have a maximum length of 128 chars.

<font color="#C27AFF">effect</font>: `<EventId>` — an `Id<EventTag>`, the same
phantom-typed id used by `SceneId`/`ObjectId`
(see [Id implementation](item.md#ids-implementation))
referencing an entry in the [Event Schema](events.md#event-schema).
---

## Effects (Damages / Heal)

This is the effects, damage or heal values, that the item which holds the data
has when it will be used against a NPC or whatever other entity (which can
receive a damage) in the game. This kind of structure let us to define more
values like magic, or whatever other that we need.

### Schema

```json
    {
  "effect": {
    "applied_to": "<uint8_t>",
    "base": "<uint8_t>",
    "critical": {
      "chance": "<uint8_t>",
      "multiplier": "<uint8_t>"
    },
    "rates": {
      "rate": "<uint8_t>",
      "unit": "<uint8_t>"
    }
  }
}
```

### Details
<font color="#C27AFF">applied_to</font>: This is the base effect value when it is
applied. Always is applied following this schema: 

```
    7 6 5 4 3 2 1 0
    ---------------
    | | | | | | | |
    | | | | | | | |-> life (capacities.vitalities.life / service_life)
    | | | | | | |---> stamina (capacities.vitalities.stamina)
    | | | | | |-----> sanity (capacities.vitalities.sanity)
    | | | | |-------> load (capacities.load)
    | | | |---------> Reserved
    | | |-----------> Reserved
    | |-------------> Reserved
    |---------------> Reserved
```

<font color="#C27AFF">base</font>: This is the base effect value when it is
applied.  
<font color="#C27AFF">critical.chance</font>: This is the chance to get a
*critical* damage which will apply the maximum damage value to the target of
damage.  
<font color="#C27AFF">critical.multiplier</font>: This is the multiplier to be
applied, just in case the main character have one.  
<font color="#C27AFF">rates.rate</font>: This is the unit ratio used to apply
the value.  
<font color="#C27AFF">rate.unit</font>: This the unit used to apply the rate and
it is defined with the following bitmap.

```
     7 6 5 4 3 2 1 0
    ------------------
     | | | | | | | |
     | | | | | | | ---> Turns
     | | | | | | -----> Seconds
     | | | | | -------> Reserved
     | | | | ---------> Reserved
     | | | -----------> Reserved
     | | -------------> User Defined
     | ---------------> User Defined
     -----------------> User Defined
```


