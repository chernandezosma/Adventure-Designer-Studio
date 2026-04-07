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

## Purpose

This folder contains the technical documentation for the Adventure Designer Studio project.
Each document inside `docs/core/` describes one specific area of the application: its
responsibilities, the design decisions behind it, and the relationships it has with other
parts of the system.

The goal is to give any contributor — or your future self — a clear mental model of how
the project is structured before diving into the source code.

## What you will find here

- `data.md` — DataObject layer: the serialisable game-content structs.
- `entities.md` — Entity adapters: the inspector-facing wrappers over DataObjects.
- `project.md` — `Core::Project`: the top-level container that owns everything.
- `ide.md` — IDE panels and the rendering pipeline.
- `navigation.md` — File dialogs, open/save flow, and the deferred-dialog pattern.

## Conventions used across all documents

- **DataObject** — a plain struct that holds game data and knows nothing about the UI.
  These are the structs that get written to and read from the `.ads` file on disk.
- **Entity adapter** — a class that wraps a DataObject pointer and exposes the data in a
  form convenient for the inspector panel. Entities are rebuilt from DataObjects on load
  and are never serialised directly.
- **`ADS::` namespace prefix** — all production code lives under the `ADS` namespace,
  with sub-namespaces (`ADS::Core`, `ADS::Data`, `ADS::Entities`, `ADS::IDE`) matching
  the folder layout under `src/classes/`.