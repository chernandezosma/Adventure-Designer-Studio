<!--
Copyright (C) 2026 [Cayetano H. Osma](cayetano.hernandez.osma@gmail.com)

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

# Translations — the `.trn` file

## What it is

Every project that is saved to `<name>.ads` also gets a sibling
`<name>.trn` file in the same folder. The `.trn` file holds the game's
**authored translatable strings**, one value per language the project supports.

It is a separate, standalone artifact:

- It is **not** the IDE's own interface translations — those live in
  `public/translations/core/*.json` and are unrelated.
- It is **not** covered by the `.ads` integrity checksum. A `.trn` file is
  meant to be handed to a translator and edited by hand.
- A missing `.trn` is not an error: a project that has never had translatable
  text simply has no file yet.

The strings are edited in the IDE through the **Translations** panel
(`View ▸ Translations`, or the toolbar button), which shows every translatable
string grouped by where it appears, with one text area per project language.

## Which strings

A string is translatable when its field carries the `translatable` marker (in
the code, `PropertyConstraints::translatable`). Today that is:

| Entity type | Fields |
|---|---|
| Scene, State, Character, Item | `name` |
| Scene, State, Character, Item | the four Descriptions slots: `normal`, `long`, `odor`, `sound` |

The `long` / `odor` / `sound` slots follow
[`common-structures.md`](common-structures.md#decriptions-definition). Item
`synonyms` / `abbreviatures` and the game `synopsis` / trigger text are **not
yet** covered and are a planned addition.

## String ids

Each translatable string has a stable id of the form:

```
<type>.<numericId>.<field>
```

- `<type>` — `scene` | `state` | `character` | `item`
- `<numericId>` — the entity's `Id<Tag>` value (the same number used in `.ads`)
- `<field>` — `name` | `desc.normal` | `desc.long` | `desc.odor` | `desc.sound`

Examples: `scene.1.name`, `item.3.desc.normal`, `character.2.desc.sound`.

An id never changes for the life of an entity — renaming the entity does not
change its id.

## File format

```json
{
  "format": "ads-translations",
  "schemaVersion": 1,
  "languages": {
    "en_US": {
      "scene.1.name": "The entrance",
      "scene.1.desc.normal": "A stone doorway, worn smooth."
    },
    "es_ES": {
      "scene.1.name": "La entrada",
      "scene.1.desc.normal": "Un umbral de piedra, pulido por el tiempo."
    }
  }
}
```

### Details

<font color="#C27AFF">format</font>: always `"ads-translations"`.

<font color="#C27AFF">schemaVersion</font>: `1`. A file whose version the IDE
does not recognise is ignored with a warning (the project loads, translations
stay empty).

<font color="#C27AFF">languages</font>: an object keyed by
BCP-47/POSIX language code, matching the
[Language catalog](game.md#language-catalog) in
`src/constants/languages.h`. The keys are the codes for the ids in
`game.languages.supported` — see [`game.md`](game.md#game-definition). Each
value is an object mapping a string id to its text in that language. A string
with no text in a language is simply absent from that language's object.

For a `name` id, the entry under the project's **default** language
(`game.languages.default`) mirrors the entity's plain name as stored in `.ads`
— so a build reads the name from one uniform place regardless of the target
language.

## How it is used

The `.trn` file is the standalone authored-translation source. At compile time
the [LexEngine](lexengine/LexEngine.md) reads the strings belonging to the
**target language** chosen for that build and processes them through its
vocabulary/tokenisation pipeline; the compiled output contains token-id arrays,
never the raw strings (see
[`common-structures.md`](common-structures.md)). Building the LexEngine from
`.trn` is not yet wired.

## Related code

- `src/classes/Core/TranslationCatalog.{h,cpp}` — enumerates the translatable
  strings of a `Core::Project` and routes `get` / `set` of a string id to the
  DataObject field that backs it.
- `src/classes/Core/TranslationSerializer.{h,cpp}` — reads / writes the `.trn`
  file. Called from `Core::ProjectSerializer::save` / `load`.
- `src/classes/IDE/panels/TranslationPanel.{h,cpp}` — the two-pane editor.
