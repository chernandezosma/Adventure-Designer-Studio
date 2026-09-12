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

# Items

Items are defined as any object, weapon, and, more generally, any element
present in the game with which the user can interact. Such interaction may be
limited to simple observation or inclusion in a description, or it may involve
actions requiring a higher level of interaction with the item.

In this line, a table is an object if the programmer wants to be examinable,
touchable, or whatever any other action that match with

## Item definition

This is the first approach for the item's schema:

```json
{
  "items": [
    {
      "id": "<uint8_t>",
      "name": "<Text, 128>",
      "descriptions": "<desriptions>",
      "language": {
        "synonyms": "<uint16_t>",
        "abbreviatures": "<uint16_t>"
      },
      "image": "<String> | Filename or string encoded",
      "affordance": "<affordance[]>",
      "is_container": "<bool>",
      "state": "<uint8_t>",
      "weight": "<uint8_t>",
      "slots": "<uint8_t>",
      "service_life": "<uint8_t>",
      "effects": {
        "damage": "<effect>",
        "heal": "<effect>"
      },
      "container": "<uint16_t>",
      "combinable_with": "<uint16_t>",
      "triggers": "<Triggers> — see common-structures.md#global-triggers"
    }
  ]
}
```

### Details

Several of the entries above require further explanation; these are detailed
below.

<font color="#C27AFF">id</font>: This is a unique value (`uint8_t`) for each
item's type. See
[implementation](#Ids-implementation) section to see how to manage them.

<font color="#C27AFF">name</font>: This is the human readable item's name. It
will show in the game as the item's name. It will be treated as a string with a
max length of 128 bytes, but internally will be stored as <uint32_t>, and it
represents a LexEngine id.

<font color="#C27AFF">descriptions</font>: This is the group of extra texts
which will describe the item. See the shared
doc [Texts definition](common-structures.md#decriptions-definition)

<font color="#C27AFF">image</font>: It will be a filename or image encoding. It
is a String.

<font color="#C27AFF">affordance</font>: An array of `<affordance>` entries — see
[Affordances definitions](#Affordances-definitions) below. Each entry names
what the player can do with the item (e.g. "Takeable") and the trigger names
it fires (e.g. `on_pickup`, `on_drop`). The author defines and is
responsible for resolving these trigger names; the schema only stores the
pairing.

<font color="#C27AFF">is_container</font>: A `<bool>` marking whether the
item is a container. Independent of `affordance` — not itself an
affordance entry. Only meaningful together with `container` below.

<font color="#C27AFF">state</font>: it is an `<uint8_t>` holding the `id` of the
item's single currently-active state entry, `0` = `None`. The detailed
description is in
[States](#states), following the rules for
shared [State Catalog Pattern](common-structures.md#states).

<font color="#C27AFF">weight</font>: What is the weight? It must be a <uint8_t>
value from 0 lightest to 255 heaviest. Probably a 255 value implies that the
object is neither takeable nor, obviously, moveable.

<font color="#C27AFF">slots</font>: Number of places that the item uses into the
inventory.

<font color="#C27AFF">language</font>: The contained options, **synonyms** and
**abbreviatures** are the same structure, that is an array of `<String>` until
the game needs to be compiled for the target platform, and in that moment the
tetx will be translated as a pointer (`uint16_t`) which point to a collection of
LexEngine Ids which represents the words are in.

<font color="#C27AFF">service_life</font>: This is the item's life indicator, when it
reach 0, the item will be destroyed.

<font color="#C27AFF">effects</font>: is a structure which contains the heal,
damage or whatever other effect which can be applied on every single object and
mainly, affect the object's life capacity. They are represented as `<effects>`
structure and are ruled by
the [Effects](common-structures.md#effects-damages--heal)
section in common-structures file.

<font color="#C27AFF">container</font>: It is a pointer <uint16_t> to a list of
objects' IDs that are inside the current object. NULL if it is empty.

<font color="#C27AFF">combinable_with</font>: This is a pointer `<uint16_t>`
with a list of elements which can be combinable with this. Each entry is the
elements. Is not possible to get elements from different entry.

<font color="#C27AFF">triggers</font>: Map from an item-applicable global
trigger id to an array of `EventId`s referencing entries in
[Event Schema](event.md#schema). See
[Global triggers](common-structures.md#global-triggers) for the ids
applicable to items (`on_examine`, `on_item_taken`, `on_item_dropped`,
`on_item_used`) — an independent map from Scene's own `triggers`, even
though both may listen for the same global id.

## Affordances definitions

An `affordance` entry pairs an author-chosen name with the trigger names it
fires:

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

`name` is free text — not a fixed enum — but the following names are
offered as a starting preset in the inspector, carried over from the
former fixed bitmap: Takeable, Droppable, Wearable, Consumable, Openable,
Lockable, Breakable, Movable, Readable, Lightable, Throwable, Giveable,
Combinable, Fillable, Burnable, Cuttable, Writable, Listenable, Smellable,
Examinable, Showable, Hideable, Single use, Magic. An author may add,
rename, or remove entries freely, and each entry may list any number of
trigger names (e.g. `Takeable` firing both `on_pickup` and `on_drop`). The
author is responsible for defining the trigger names and for the logic
that later resolves them — this schema only stores the pairing. "Is a
container" is not an affordance entry; it is the separate `is_container`
boolean field (see above).

### Global triggers

See the [Global Triggers](common-structures.md#global-triggers) There are defined
all triggers and its corresponding to each entity.
### Considerations

To decode the `critical_chance` and `critical_multiplier`, we will use the
following code

```c++
float decode(uint8_t b) {
    return (b / 10.0f);
}
```

And we store the values as integers, that is, we remove the decimal point and
store them as is. For example, a value of 8.3 will be stored as 83, which saves
a lot of memory without using float data types.
