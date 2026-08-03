# Adventure Designer Studio — Changelog

All notable changes to this project are documented in this file.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

---

## [2026-04-29] — SDL3 migration · ImGui 1.92.7 · HiDPI fix

### Added
- `DISPLAY_SCALE` env-var override in `.env` — allows hard-coding the UI
  scale factor on X11 setups where SDL3 cannot detect HiDPI automatically
  (e.g. `DISPLAY_SCALE=2.0` for 4K at 200 %).
- `lib/imgui/` vendored at tag `v1.92.7-docking` (git submodule); built as
  `imgui_local` static library from CMakeLists.txt — includes the SDL3
  renderer and platform backends.

### Changed
- **SDL2 → SDL3** full migration across the entire codebase:
  - All `#include <SDL_*.h>` replaced with `#include <SDL3/SDL_*.h>`.
  - Window creation API: `SDL_CreateWindow(title, x, y, w, h, flags)` →
    `SDL_CreateWindow(title, w, h, flags)` + `SDL_SetWindowPosition`.
  - Renderer creation: `SDL_CreateRenderer(window, -1, flags)` →
    `SDL_CreateRenderer(window, NULL)` + `SDL_SetRenderVSync`.
  - Event system: `SDL_QUIT` / `SDL_WINDOWEVENT` constants replaced with
    `SDL_EVENT_QUIT` / `SDL_EVENT_WINDOW_CLOSE_REQUESTED`.
  - ImGui backends: `imgui_impl_sdl2` + `imgui_impl_sdlrenderer2` →
    `imgui_impl_sdl3` + `imgui_impl_sdlrenderer3`.
  - `SDL_Init`: return-value check updated from `!= 0` (SDL2 int) to `!`
    (SDL3 bool, `true` = success).
  - `SDL_INIT_GAMECONTROLLER` → `SDL_INIT_GAMEPAD`.
  - `SDL_SetRenderScale` replaces `SDL_RenderSetScale`.
  - Removed `SDL_SetMainReady()` (no longer exists in SDL3; `SDL_MAIN_HANDLED`
    macro is sufficient).
  - Window flag `SDL_WINDOW_ALLOW_HIGHDPI` replaced by
    `SDL_WINDOW_HIGH_PIXEL_DENSITY`.
  - macOS HiDPI hint removed (not present in SDL3).
- **Dear ImGui 1.91.8 → 1.92.7-docking** (vendored, replaces vcpkg package):
  - `imgui` removed from `vcpkg.json`; `sdl2` replaced with `sdl3`.
  - `imgui_local` static library target added to `CMakeLists.txt`.
  - `ImGui::PushFont(font)` (single-argument, removed in 1.92) updated to
    `ImGui::PushFont(font, font->LegacySize)` in `ToolBarRenderer.cpp`.
- `Window::setDPIScale()` replaced single `SDL_GetWindowDisplayScale` call
  with a three-level cascade (see **Fixed** below).

### Fixed
- **4K/HiDPI font scaling on X11** restored after SDL3 migration.
  `SDL_GetDisplayDPI` (SDL2, always returned physical DPI) was removed from
  SDL3. Its replacement `SDL_GetWindowDisplayScale` returns 1.0 on X11
  without `Xft.dpi` configured, causing all fonts to appear tiny on 4K
  monitors. `Window::setDPIScale()` now uses a cascade:
  1. `SDL_GetWindowDisplayScale` — Wayland, macOS, Windows, X11 + Xft.dpi.
  2. `SDL_GetWindowPixelDensity` — HiDPI X11 framebuffer (KDE/GNOME scaling).
  3. `DISPLAY_SCALE` env-var — last-resort manual override.
- `SceneData::setFakeProperty` spurious `const` qualifier removed (prevented
  writing to the member in a non-const context).
- `Scene::getFakeProperty` declaration/definition `const bool` mismatch
  corrected to `bool`.

---

## [2026-04-07] — DataObjects branch · Inspector wired to entities

### Added
- DataObject layer: `Data::BaseData`, `Data::SceneData`, `Data::CharacterData`,
  `Data::ItemData` — pure C++ structs with private members, `[[nodiscard]]`
  getters/setters, and full Doxygen comments. No ImGui dependency.
- `Types::Color` — portable RGBA color type; `ImVec4` conversion kept at the
  entity layer only.
- `BaseEntity::setAndNotify<T>` template — eliminates boilerplate from typed
  property setters; fires `PropertyEvent` only when the value actually changes.
- All entity typed setters refactored to use `setAndNotify`.
- `Core::Project` updated to own both DataObject collections and entity adapter
  collections via `std::unique_ptr`; uses DataObject accessors throughout.
- Schema docs: `docs/core/schemas/scene.md`, `docs/core/schemas/item.md`.
- Lexicon docs: `docs/core/lexicon/vocabulary.md`, `docs/core/lexicon/Lexingine.md`.

### Changed
- CMakeLists.txt updated to include the new `Data/` and `Types/` source trees.
- `CHANGES.md` updated to reflect DataObjects work items.
- `COMPILE.md` updated with revised build instructions.

---

## [2026-03-14] — Inspector · NFD · Project entity (PR #6)

### Added
- `IInspectable` interface, `PropertyDescriptor`, `PropertyConstraints`,
  `PropertyValue`, `PropertyType`, `PropertyEvent`, `PropertyEditorRegistry`.
- Property editors: `StringEditor`, `IntEditor`, `FloatEditor`, `BoolEditor`,
  `ColorEditor`, `EnumEditor`, `Vector2Editor`.
- `InspectorPanel` — property grid; renders per-category editors for the
  selected entity; wired to `Scene`, `Character`, and `Item` adapters.
- `Core::Project` — owns scenes, characters, and items; provides full CRUD
  (`addScene`/`removeScene`/`findScene`, equivalents for Character and Item),
  file-path tracking (`isSaved`, `getFilePath`), and DataObject accessors for
  serialisation.
- Native file dialog integration (nativefiledialog-extended).
- Deferred dialog pattern — NFD calls happen after `SDL_RenderPresent` to
  prevent gray-window freeze on Linux
  (`m_pendingOpenDialog` / `m_pendingSaveDialog` flags in `NavigationService`).
- "New project" unsaved-changes confirmation modal (Save / Discard / Cancel).
- File → Open Project, File → Save Project menu actions wired end-to-end.

### Changed
- `EntitiesPanel` wired to inspector via selection callback.
- `IDERenderer` passes project callbacks and file callbacks to `MenuBarRenderer`
  → `NavigationService`.

---

## [2026-02-02] — Initial inspector scaffolding

### Added
- Inspector panel skeleton with `InspectorPanel` class.
- `Scene`, `Character`, `Item` entity adapters with `getPropertyDescriptors`,
  `getPropertyValue`, and `setPropertyValue` stubs.
- Translations updated (en_US, es_ES, de_DE, fr_FR, it_IT, pt_PT, ru_RU).
- Empty docked windows removed; tab bar removed for cleaner layout.

---

## [2026-01-22] — DPI, toolbar, build copy

### Added
- `.env` file now copied to build output directory by CMake.
- `public/` folder mirrored to build directory.

### Fixed
- Overlapping docked windows on 4K monitors and Windows resolved by setting
  `WindowRounding = 0` and `WindowBg.w = 1` when viewports are enabled.
- Conditional compilation guard for toolbar spacing prevents layout shift on
  platforms that don't expose a system menu bar height.

---

## [2026-01-21] — README / COMPILE rewrite · Windows fixes

### Added
- `COMPILE.md` created as a standalone build-and-run guide (separated from
  the project README).
- `README.md` rewritten to focus on project vision and contributor context.

### Fixed
- Various compilation issues on Windows (MSVC path separators, `#ifdef _WIN32`
  guards for `SetProcessDPIAware`, `windows.h` include order).
- `.vsconfig` added so Visual Studio auto-installs the required workloads.

---

## [2026-01-19] — SDL window struct · Windows compliance

### Changed
- `SDL_WINDOW_INFO` members changed from `float` to `int` (prevents precision
  loss when passing `SDL_WINDOWPOS_CENTERED`).
- `Window` constructor signature updated from `float` to `int` parameters.
- Structs use C++ `struct Name {}` style instead of `typedef struct {} Name`.
- `SDL_WINDOW_SHOWN` added to `DEFAULT_FLAGS`.
- `SDL_FLAGS` members zero-initialised by default.

### Fixed
- `public/` folder not present in build output — CMake now copies it.
- Exceptions propagated through `main` with `SDL_ShowSimpleMessageBox` fallback.

---

## [2026-01-17] — UI refactor (PR #5)

### Changed
- Entire UI layer refactored into `ADS::UI` namespace with `ImGuiManager`,
  `Window`, `Fonts`, and `SDL_FLAGS` / `SDL_WINDOW_INFO` helper types.
- `NavigationService` created to decouple toolbar/menu event handlers from
  the renderer classes.
- `MenuBarRenderer` and `ToolBarRenderer` split from the monolithic
  `IDERenderer`; each receives a `LayoutManager*` for layout operations.
- Useless includes removed across the codebase.

---

## [2026-01-02] — Multiplatform merge (PR #4)

### Added
- Full multiplatform support: Linux, macOS, Windows.
- `build.sh` (Linux/macOS) and `build.bat` (Windows) build scripts supporting
  Release, Debug, and Test configurations.
- Logs and IDE settings files added to `.gitignore`.

---

## [2025-12-27] — Themes · Panels · LayoutManager · i18n fix

### Added
- Theme system: `IDE::Theme` base class, `DarkTheme`, `LightTheme`; applied at
  startup and switchable from Options → Theme menu.
- `LayoutManager` — manages docked panel visibility and layout reset.
- `BasePanel`, `StatusBarPanel`, `WorkingAreaPanel` panel classes.
- New exceptions under `imgui/`.
- `adsString` utility functions.
- Translation fix for a key missing from the i18n JSON files.

### Changed
- `App` class moved to `src/` root for easier global access.

---

## [2025-12-19] — Full OOP integration

### Changed
- All rendering and application logic moved into the class hierarchy.
- `main.cpp` reduced to construction, setup, `app->run()`, and cleanup.
- Main loop (`App::run()`) handles the SDL event pump, ImGui frame lifecycle,
  and `SDL_RenderPresent`.

---

## [2025-12-18] — Fonts class · Pre-loop classes

### Added
- `Fonts` class — manages font loading, merging (FontAwesome icons), and lookup
  by name; `loadDefaultFonts()`, `loadFontFromFile()`, `loadIconFont()`.
- Pre-main-loop initialisation (SDL, ImGui context, backends) moved into
  `ImGuiManager::init()`.
- `.clang-format` (LLVM style) added.

---

## [2025-12-08] — UI, Window, Logging, UUID (submodules)

### Added
- `UI` class (`ImGuiManager`) and `Window` class with SDL renderer management.
- Logging via `spdlog`; debug log level controlled by `DEBUG` in `.env`.
- UUID generation using `boost-uuid`.
- Git submodules initialised (`lib/`).

---

## [2025-11-26] — ImGui library integration

### Added
- Dear ImGui added to `lib/` as a git submodule.
- Initial docking layout skeleton.

---

## [2025-10-15] — Foundation (PR #1 · i18n + UI bootstrap)

### Added
- CMake project scaffold with Release / Debug / Test build configurations.
- SDL2 + Dear ImGui render loop (immediate mode).
- i18n system (`i18nUtils`, `TranslationManager`) — JSON-based, supports
  en_US, es_ES, de_DE, fr_FR, it_IT, pt_PT, ru_RU.
- `.env` support (`Environment` class) with `DEBUG`, `FONT_SIZE_BASE`,
  `LIGHT_FONT`, `MEDIUM_FONT`, `REGULAR_FONT`, `TOOLTIP_DELAY` keys.
- System constants (`System.h`), `ADS_DEBUG` compile-time guard.
- FontAwesome icon font integration.
- ImGui settings persistence (`imgui.ini` path from `System::CONFIG_FILE`).
- Google Test framework integration (optional via `BUILD_TESTING_ENABLED`).
- `.clang-format` (LLVM style).
- `vcpkg.json` manifest with `builtin-baseline` and vcpkg overlay ports for
  `libsystemd` GCC 15 compatibility fix.

---

## [2025-09-22] — i18n tests

### Added
- Unit tests for `TranslationManager` / `i18n` using Google Test.

### Fixed
- Several translation lookup edge cases.