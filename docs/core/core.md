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

# Core Documentation

## Notes

In the documents you will see some items in <font color="#008000">
future-color</font>
and other in <font color="#C27AFF">normal-item-color</font>.

The <font color="#008000">Future Items</font> are features that will be
implemented in a future, but now are not. Stay tuned to the blog. ;-)

On the other hand the <font color="#C27AFF">normal-item-color</font> are items
that belongs to a list of items which will be explained in detail.

## Purpose

This folder contains the technical documentation for the Adventure Designer
Studio project. Each document inside `docs/core/` describes one specific area of
the application: its responsibilities, the design decisions behind it, and the
relationships it has with other parts of the system.

The goal is to give any contributor — or your future self — a clear mental model
of how the project is structured before diving into the source code.

## What you will find here

### Planned (not yet written)

- `data.md` — DataObject layer: the serialisable game-content structs.
- `entities.md` — Entity adapters: the inspector-facing wrappers over
  DataObjects.
- `project.md` — `Core::Project`: the top-level container that owns everything.
- `ide.md` — IDE panels and the rendering pipeline.
- `navigation.md` — File dialogs, open/save flow, and the deferred-dialog
  pattern.

### Design notes & open questions

- [`questions.md`](questions.md) — Open design decisions still pending for the
  engine/IDE.

### LexEngine

- [`lexengine/LexEngine.md`](lexengine/LexEngine.md) — LexEngine: the dual
  lexicon+engine system for authoring-time NLP and runtime tokenized dispatch on
  8-bit targets.
- [`lexengine/vocabulary.md`](lexengine/vocabulary.md) — LexEngine system design:
  vocabulary/word-frequency management and tokenized binary output for
  compilation.

### Schemas

- [`schemas/character.md`](schemas/character.md) — Character schema: player
  stats, capacities, and the states catalog (physical, mental, environmental,
  metaphysical).
- [`schemas/common-structures.md`](schemas/common-structures.md) — Shared JSON
  structures (descriptions, state catalog pattern, etc.) reused across other
  schemas.
- [`schemas/core-routines.md`](schemas/core-routines.md) — Core Routines:
  platform-specific ASM primitives (like DAAD Condactos) chained by the event
  system.
- [`schemas/event.md`](schemas/event.md) — Events: named signals emitted by the
  engine that triggers react to.
- [`schemas/item.md`](schemas/item.md) — Item schema: any interactable
  object/weapon/element in the game world.
- [`schemas/scene.md`](schemas/scene.md) — Scene schema: a game location —
  container for items, exits, and triggers.

## Conventions used across all documents

- **DataObject** — a plain struct that holds game data and knows nothing about
  the UI. These are the structs that get written to and read from the `.ads`
  file on disk.
- **Entity adapter** — a class that wraps a DataObject pointer and exposes the
  data in a form convenient for the inspector panel. Entities are rebuilt from
  DataObjects on load and are never serialised directly.
- **`ADS::` namespace prefix** — all production code lives under the `ADS`
  namespace, with sub-namespaces (`ADS::Core`, `ADS::Data`, `ADS::Entities`,
  `ADS::IDE`) matching the folder layout under `src/classes/`.