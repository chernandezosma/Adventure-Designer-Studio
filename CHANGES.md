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
- [x] DataObject collections (`getSceneData`, `getCharacterData`, `getItemData`) for serialisation
- [ ] Project serialisation — save project to `.ads` file
- [ ] Project deserialisation — load project from `.ads` file

### DataObject Layer

- [x] `Types::Color` — portable RGBA color type (no ImGui dependency)
- [x] `Data::BaseData` — common id + name base for all DataObjects
- [x] `Data::SceneData` — pure data struct for scenes (exits, backgroundImagePath, musicPath, etc.)
- [x] `Data::CharacterData` — pure data struct for characters (portraitPath, startingSceneId, etc.)
- [x] `Data::ItemData` — pure data struct for items (iconPath, startingSceneId, etc.)
- [x] DataObjects converted from `struct` (public members) to `class` (private members + public getters/setters with `[[nodiscard]]` and full Doxygen)
- [x] `Project.cpp` updated to use DataObject accessors (`setId`, `setName`, `getId`) everywhere

### Entities

- [x] `BaseEntity` — inspector adapter base; delegates id/name storage to `Data::BaseData`
- [x] `Scene` entity — inspector adapter backed by `Data::SceneData`; adds backgroundImagePath, musicPath
- [x] `Character` entity — inspector adapter backed by `Data::CharacterData`; adds portraitPath, startingSceneId
- [x] `Item` entity — inspector adapter backed by `Data::ItemData`; adds iconPath, startingSceneId
- [x] `BaseEntity::setAndNotify<T>` template — eliminates boilerplate from all typed property setters (bool, int, std::string); Color and EnumValue setters remain manual due to type conversion
- [x] All entity typed setters (`setDescription`, `setWidth`, `setHealth`, etc.) refactored to use `setAndNotify`
- [x] Full Doxygen comments applied to all public methods across all entity headers

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

- [x] Scene inspector — backgroundImagePath, musicPath, backgroundColor, dimensions, isStartScene
- [x] Character inspector — portraitPath, startingSceneId, health, maxHealth, dialogColor
- [x] Item inspector — iconPath, startingSceneId, itemType, isPickable, isUsable, quantity
- [ ] Complex list UI — exits (scene connections), inventory items, present characters/items per scene

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
