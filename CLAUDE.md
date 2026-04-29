# CLAUDE.md — Adventure Designer Studio

Project-level instructions for Claude Code. These override default behaviour.

---

## Project snapshot

C++23 / SDL2 / ImGui IDE for designing conversational adventure games.
**Active branch:** `DataObjects` — PR target: `main`
**Build system:** CMake + vcpkg (manifest mode, toolchain at `/opt/vcpkg`)

---

## Searching files — critical rules

`lib/` contains hundreds of files (imgui, nlohmann_json, spdlog…) and will swamp
any broad search, truncating results before root-level files appear.

- **Root `.md` files** → `Glob("*.md", path="<project_root>")` or just `Read` by name
- **Docs subtree** → `Glob("docs/**/*.md")`
- **Source files** → `Glob("src/**/*.{cpp,h}")`
- **Never** use bare `*.md` or `**/*.md` without a scoped path

Key root-level files to know without searching:
`COMPILE.md`, `CHANGES.md`, `README.md`, `LIBRARY_SETUP.md`, `VCPKG_MIGRATION.md`,
`CMakeLists.txt`, `vcpkg.json`, `vcpkg-configuration.json`, `build.sh`, `build.bat`

---

## Architecture — DataObject / Entity split

```
Data layer (owns data, no ImGui)        Entity layer (inspector adapters)
─────────────────────────────────       ──────────────────────────────────
Data::BaseData          ←───────────── Entities::BaseEntity
Data::SceneData         ←───────────── Entities::Scene
Data::CharacterData     ←───────────── Entities::Character
Data::ItemData          ←───────────── Entities::Item
```

- `Core::Project` owns **both** collections via `std::unique_ptr`
- Entities hold **non-owning raw pointers** into the DataObjects
- `ADS::Types::Color` ↔ `ImVec4` conversion happens **only** at entity getters/setters
- DataObjects never include ImGui headers

---

## Key source locations

| What | Where |
|---|---|
| App loop | `src/app.cpp` — `App::run()` |
| Portable color type | `src/classes/Types/Color.h` |
| DataObjects | `src/classes/Data/` |
| Entity adapters | `src/classes/Entities/` |
| Project container | `src/classes/Core/Project.cpp/.h` |
| IDE top-level | `src/classes/IDE/IDERenderer.cpp/.h` |
| Menu bar | `src/classes/IDE/navigation/MenuBarRenderer.cpp/.h` |
| File dialog logic | `src/classes/IDE/navigation/NavigationService.cpp/.h` |
| Entity list panel | `src/classes/IDE/panels/EntitiesPanel.cpp/.h` |
| Inspector panel | `src/classes/IDE/panels/InspectorPanel.cpp/.h` |

---

## Deferred dialog pattern

Native file dialogs (NFD) must **never** be opened inline during rendering.
The correct chain is:

```
App::run()
  → render()
  → SDL_RenderPresent()
  → processPendingDialogs()   ← NFD calls happen here only
```

Flags: `m_pendingOpenDialog`, `m_pendingSaveDialog` in `NavigationService`.
Reason: opening NFD inside the render loop causes a gray-window freeze on Linux.

---

## Coding standards

- **C++ standard:** C++23
- **Namespaces:** `ADS::` root; sub-namespaces `ADS::Core`, `ADS::Data`, `ADS::Entities`, `ADS::IDE`
- **Docstrings:** full Doxygen-style on **all** public methods — no exceptions
- **No ImGui types** in DataObjects or anything below the Entity layer
- **Author header:** `Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>`

---

## Build — known issues and fixes

### Linux system packages required

SDL2 is built by vcpkg but links against system graphics/input libraries.
Required (Debian/Ubuntu):
```
libx11-dev libxft-dev libxext-dev libwayland-dev
libxkbcommon-dev libegl1-mesa-dev libibus-1.0-dev
```

### GCC 15 / libsystemd

vcpkg's `libsystemd` fails with GCC 15 (`-Werror=override-init` in `errno-to-name.h`).
**Fix is already in the repo** at `vcpkg-overlays/libsystemd/portfile.cmake` and wired
via `vcpkg-configuration.json` → `overlay-ports`. No manual action needed.

---

## Documentation state

| File | Status |
|---|---|
| `docs/core/core.md` | Exists — docs index |
| `docs/core/lexicon/vocabulary.md` | Exists |
| `docs/core/schemas/item.md` | Exists |
| `docs/core/data.md` | **Missing** |
| `docs/core/entities.md` | **Missing** |
| `docs/core/project.md` | **Missing** |
| `docs/core/ide.md` | **Missing** |
| `docs/core/navigation.md` | **Missing** |

---

## Next work items

- [ ] Project serialisation — save DataObjects to `.ads` via `nlohmann_json`
- [ ] Project deserialisation — load `.ads`, rebuild entity adapters
- [ ] Complex list UI — exits, inventory, present characters/items per scene
- [ ] File → Save As
- [ ] Node editor panel
- [ ] Write missing docs under `docs/core/`
