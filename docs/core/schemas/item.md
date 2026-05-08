
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

Items are defined as all objects, weapons, and, more generally, any element present in the game with which the user can interact. Such interaction may be limited to simple observation or inclusion in a description, or it may involve actions requiring a higher level of interaction with the item.

## Item definition

This is the first approach for the item's schema:
```json
{
    "objects": [
    {
        "id": "<uint8_t>",
        "name": "<String> | max: 128",
        "descriptions": {
            "description": "<String> | max: 128",
            "long_description": "<string> | max: 255",
            "odor_description": "<string> | max: 255",
            "sound_description": "<string> | max: 255"
        },
        "image": "<String> | Filename or string encoded",
        "affordances": "<uint16_t>",
        "state": "<uint8_t>",
        "interactions": "<uint16_t>",
        "weight": "<uint8_t>",
        "language": {
          "synonyms": "<uint16_t>",
          "abbreviatures": "<uint16_t>"
        },
        "damage": {
            "base_damage": "<uint8_t>",
            "critical_chance": "<uint8_t>",
            "critical_multiplier": "<uint8_t>"
        },
        "effectivity": "<uint8_t>",
        "container": "<uint16_t>",
        "combinable_with": "<uint16_t>"
    }
  ]
}
```
### Details

Several of the entries above require further explanation; these are detailed below.

- **id**: This is a unique value (byte) for each item's type. See [implementation](#Ids-implementation) section to see how to manage them.
- **name**: This is the item's name. It will show in the game as the item's name. It will be a string with a max length of 128 bytes.
- **descriptions**: This is the group of extra texts which will describe the item. 
  - **description**": Text which describes the item briefly, it is a string with a max length of 128 bytes.
  - **long_description**: Text which describes, in detail, the item; it is a string with a max length of 255 bytes.
  - **odor_description**: Text which describes, in detail, how the item smells, it is a string with a max length of 255 bytes.
  - **sound_description**: Text which describes, in detail, how the item sounds. It is a string with a max length of 255 bytes. 
- **image**: It will be a filename or image encoding. It is a String.
- **affordables**: This is a pointer to a <uint32_t> memory area with the structure defined at [Affordances](#Affordances-definition) section, and its implementation info at [Affordance implementation](affordance-impplementation).
- **state**: It is a pointer to a `<uint16_t>` memory area where some affordances state will be stored. The detailed description is in [States](States), and its implementation is in [States implementation](states-implementation).
- **interactions**: Pointer to the `<uint16_t>`,
- **weight**: What is the weight? It must be a <uint8_t> value from 0 lightest to 255 heaviest. Probably a 255 value implies that the object is neither  takeable nor, obviously, moveable.
- **language**: 
  - **synonyms**: Ponter (`uint16_t`), to the language structure where the words are in.
  - **abbreviatures**: Pointer (`<uint16_t>`), to the abbreviatures structure where they are stored.  
"**damage**": {
  "**base**": "`<uint8_t>`",
  "**critical_chance**": "`<uint8_t>`",
  "**critical_multiplier**": "`<uint8_t>`"
},
"**effectivity**": "`<uint8_t>`",
"**container**": It is a pointer <uint16_t> to a list of objects' IDs that are inside the current object.
"**combinable_with**": "`<uint16_t>`"

## Affordances definitions

The affordances bitmap would be:
```
    F E D C B A 9 8 7 6 5 4 3 2 1 0
    ---------------------------------
    | | | | | | | | | | | | | | | |
    | | | | | | | | | | | | | | | ---> Takeable
    | | | | | | | | | | | | | | -----> Droppable
    | | | | | | | | | | | | | -------> Wearable
    | | | | | | | | | | | | ---------> Consumable
    | | | | | | | | | | | -----------> Openable
    | | | | | | | | | | -------------> Lockable
    | | | | | | | | | ---------------> Breakable
    | | | | | | | | -----------------> Movable
    | | | | | | | -------------------> Readable
    | | | | | | ---------------------> Lightable
    | | | | | -----------------------> Throwable
    | | | | -------------------------> Giveable
    | | | ---------------------------> Combinable
    | | -----------------------------> Fillable
    | -------------------------------> Burnable
    ---------------------------------> Cuttable

    1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 	
    F E D C B A 9 8 7 6 5 4 3 2 1 0
    --------------------------------
    | | | | | | | | | | | | | | | |
    | | | | | | | | | | | | | | | |-> Writable
    | | | | | | | | | | | | | | ----> Listenable
    | | | | | | | | | | | | | ------> Smellable
    | | | | | | | | | | | | --------> Examinable
    | | | | | | | | | | | ----------> Showable
    | | | | | | | | | | ------------> Hideable
    | | | | | | | | | --------------> single use
    | | | | | | | | ----------------> Is Container
    | | | | | | | ------------------> (Reserved)
    | | | | | | --------------------> (Reserved)
    | | | | | ----------------------> (Reserved)
    | | | | ------------------------> (Reserved)
    | | | --------------------------> (User defined 1)
    | | ----------------------------> (User defined 2)
    | ------------------------------> (User defined 3)
```

## Item attributes

This is an approach to hold the object's attributes

## States

```json
{
  "states": [
    {
      "id": "<uint8_t>",
      "name": "<String> | max: 128",
      "description": "<String> | max: 128",
      "damage": {
        "base_damage": "<uint8_t>",
        "critical_chance": "<uint8_t>",
        "critical_multiplier": "<uint8_t>"
      },
      "effectivity": "<uint8_t>"
    }
  ]
}
```

## Implementation

### IDs implementation
The IDs will be implemented as follows

```c++
    template<typename Tag>
    struct Id {
        uint8_t value;
        explicit Id(uint8_t v) : value(v) {}
        bool operator==(const Id&) const = default;
    };

    // In the Project entity, we will store the IDs for each type of entity
    struct RoomTag {};
    struct ObjectTag {};
    struct EventTag {};

    using RoomId   = Id<RoomTag>;
    using ObjectId = Id<ObjectTag>;
    using EventId  = Id<EventTag>;
```

### Considerations

To decode the `critical_chance` and `critical_multiplier`, we will use the following code

```c++
float decode(uint8_t b) {
    return (b / 10.0f);
}
```

And we store the values as integers, that is, we remove the decimal point and store them as is. For example, a value of 8.3 will be stored as 83, which saves a lot of memory without using float data types.
