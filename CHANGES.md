# Adventure Designer Studio — Change Log

This file tracks implemented features and pending work.
Use `[x]` for completed items and `[ ]` for pending ones.

---

## Inspector Branch

### Core Infrastructure

- [x] Multiplatform support (Linux, Windows)
- [x] SDL2 + ImGui render loop
- [x] Docked panel layout (LayoutManager)
- [x] Navigation service (menu bar + toolbar handlers)
- [x] Translation system (en_US, es_ES, de_DE, fr_FR, it_IT, pt_PT, ru_RU)
- [x] Theme system
- [x] Logging with spdlog

### Project Entity (`Core::Project`)

- [x] Project class with name, scenes, characters and items collections
- [x] Scene CRUD (`addScene`, `removeScene`, `findScene`, `getScenes`)
- [x] Character CRUD (`addCharacter`, `removeCharacter`, `findCharacter`, `getCharacters`)
- [x] Item CRUD (`addItem`, `removeItem`, `findItem`, `getItems`)
- [x] File path tracking (`isSaved`, `getFilePath`, `setFilePath`, `clearFilePath`)
- [ ] Project serialisation — save project to `.ads` file
- [ ] Project deserialisation — load project from `.ads` file

### Entities

- [x] `BaseEntity` — common id, name, description base class
- [x] `Scene` entity — implements `IInspectable`, exposes properties via `PropertyDescriptor`
- [x] `Character` entity — implements `IInspectable`, exposes properties via `PropertyDescriptor`
- [x] `Item` entity — implements `IInspectable`, exposes properties via `PropertyDescriptor`

### Inspector System

- [x] `IInspectable` interface — contract for inspectable entities
- [x] `PropertyDescriptor` — metadata for each property (name, type, category, constraints)
- [x] `PropertyConstraints` — min/max/step/enum constraints per property
- [x] `PropertyValue` — type-safe value wrapper
- [x] `PropertyType` — enum of supported property types
- [x] `PropertyEvent` — event fired when a property value changes
- [x] `PropertyEditorRegistry` — maps property types to editor widgets

#### Property Editors

- [x] `IPropertyEditor` — base interface for all editors
- [x] `StringEditor` — text field editor
- [x] `IntEditor` — integer slider/input editor
- [x] `FloatEditor` — float slider/input editor
- [x] `BoolEditor` — checkbox editor
- [x] `ColorEditor` — RGBA color picker editor
- [x] `EnumEditor` — combo-box editor for enum values
- [x] `Vector2Editor` — two-component float editor

### IDE Panels

- [x] `BasePanel` — visibility, name, docking base class
- [x] `StatusBarPanel` — bottom status bar
- [x] `WorkingAreaPanel` — central working area placeholder
- [x] `EntitiesPanel` — entity list with selection; wired to inspector
- [x] `InspectorPanel` — property grid; renders per-category editors for the selected entity

#### Inspector Panel — Per-Entity Implementation

- [ ] Scene inspector — specific fields (background, music, transition type…)
- [ ] Character inspector — specific fields (portrait, stats, dialogue tree…)
- [ ] Item inspector — specific fields (icon, usable flag, quantity…)

### File Dialogs (NFD)

- [x] Native file dialog integration (nativefiledialog-extended)
- [x] Deferred dialog pattern — NFD calls run after `SDL_RenderPresent` to prevent gray-window artifact
- [x] Open project dialog — selects `.ads` file and forwards path to callback
- [x] Save project dialog — selects `.ads` path and forwards it to callback
- [x] "New project" confirmation modal — offers Save / Discard / Cancel before creating a new project

### Menu Bar

- [x] File → New Project (with unsaved-changes confirmation)
- [x] File → Open Project (native file picker)
- [x] File → Save Project (native file picker)
- [ ] File → Save As (distinct from Save when a path is already set)
- [ ] Edit menu actions (undo, redo, cut, copy, paste)
- [ ] Help menu (about dialog, documentation link)

### Node Editor

- [ ] Node editor panel — visual scripting canvas
- [ ] Node types (action, condition, dialogue, transition…)
- [ ] Node connections / edges
- [ ] Serialisation of node graphs

---

## Backlog / Future Work

- [ ] Undo / redo system
- [ ] Project settings dialog
- [ ] Export / build system for the adventure game
- [ ] Plugin / scripting support
- [ ] Asset manager (images, audio, fonts)
