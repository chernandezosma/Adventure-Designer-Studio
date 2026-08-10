<p align="center"><img src="public/assets/logo.png" width="564" alt="Adventure Studio Designer Logo"></p>

# Testing Guide — Adventure Designer Studio

This guide covers building and running the unit test suite, and generating
a line-coverage report. For general build setup (vcpkg, system packages,
IDE configuration), see [COMPILE.md](./COMPILE.md) first — this guide
assumes you already have a working build.

## Prerequisites

Nothing beyond what [COMPILE.md](./COMPILE.md) already sets up. `gtest` is
declared in `vcpkg.json` and is fetched automatically by CMake the first
time you configure a `Test` build — no separate install step.

## Running the test suite

The `Test` build type (`CMAKE_CXX_FLAGS_TEST`) enables GoogleTest and
compiles `tests/` into the `Adventure_Designer_Studio_Tests` executable,
registered with CTest.

<details>
<summary><strong>Linux / macOS</strong></summary>

```bash
./build.sh Test
ctest --test-dir build --output-on-failure
```

</details>

<details>
<summary><strong>Windows</strong></summary>

```batch
build.bat Test
ctest --test-dir build --output-on-failure
```

</details>

To run a single test or filter by name, pass CTest's `-R` (regex) flag:

```bash
ctest --test-dir build --output-on-failure -R i18nTests
```

Or run the test binary directly for GoogleTest's own filtering
(`--gtest_filter`, `--gtest_list_tests`, etc.):

```bash
# Linux/macOS
./build/tests/Adventure_Designer_Studio_Tests --gtest_filter=Color.*

# Windows
build\tests\Test\Adventure_Designer_Studio_Tests.exe --gtest_filter=Color.*
```

## Test layout

```
tests/
├── unit/
│   ├── types/Color/ColorTests.cpp
│   ├── data/{BaseData,SceneData,CharacterData,ItemData}/*Tests.cpp
│   └── i18n/i18n/{i18nTests.cpp,i18nTests.h}
├── integration/
└── coverage/
    └── coverage.sh
```

Each test suite gets its own folder under `tests/unit/<ClassName>/` (or
`tests/integration/<ClassName>/` for integration-style tests) — one folder
per class, `<ClassName>Tests.cpp` inside. `tests/CMakeLists.txt` globs
`*.cpp` recursively under `tests/`, so any new file placed in this layout is
picked up automatically; no CMake changes are needed to add a new test file
to an existing class folder.

Naming convention: `TEST(ClassName, Method_Scenario)` (or `TEST_F` for
fixtures), e.g. `TEST(SceneData, SetWidth_UpdatesWidth)`.

## Generating a coverage report (Linux/macOS, GCC/Clang)

The `Test` build type is compiled with `--coverage` (gcov instrumentation).
`tests/coverage/coverage.sh` builds, runs the suite, and produces an HTML
report via `lcov`/`genhtml`.

**Requires `lcov` and `genhtml`:**

```bash
# Debian/Ubuntu
sudo apt install lcov

# macOS
brew install lcov
```

> **Windows:** `lcov`/`genhtml` aren't available — they're Perl scripts
> that ship as Linux/macOS packages, and `tests/coverage/coverage.sh` is a
> bash script besides. Don't try to install `lcov` on Windows. Note that
> `--coverage` instrumentation itself is only wired up for the `GNU|Clang`
> branch in the root `CMakeLists.txt` — plain MSVC builds get no gcov data
> to report at all, regardless of tool. So coverage on Windows requires a
> GCC-based toolchain (e.g. MinGW-w64), and then either:
> - [CLion's built-in coverage runner](#viewing-coverage-in-clion), or
> - [`gcovr` for VS Code / any other editor](#viewing-coverage-in-vs-code-mingw-gcc-only).

**Run it:**

```bash
./tests/coverage/coverage.sh
```

This configures a separate `build-coverage/` directory (so it doesn't
collide with your regular `build/`), runs `ctest`, then writes both the raw
`.info` file and the HTML report under `tests/coverage/reports/`:

```
tests/coverage/reports/
├── coverage.info      # raw lcov trace file
└── html/
    └── index.html     # open this
```

```bash
# Linux
xdg-open tests/coverage/reports/html/index.html

# macOS
open tests/coverage/reports/html/index.html
```

`tests/coverage/reports/` is generated output (gitignored) — safe to delete
and regenerate at any time.

> **Note:** `gcov`/`lcov` are GCC/Clang tools and don't apply to MSVC
> builds. On Windows, use CLion's built-in coverage runner (below) or
> Visual Studio's own Code Coverage feature instead.

## Importing coverage.sh's report into CLion

`tests/coverage/reports/html/index.html` is a human-readable report for a
browser — CLion's coverage viewer doesn't open it. Point CLion at the raw
LCOV tracefile instead:

1. **Run → Show Coverage Data…** (or the coverage icon in the toolbar).
2. Click **+** to add an external suite.
3. Select `tests/coverage/reports/coverage.info`.

CLion parses the tracefile itself and shows the same file-tree percentages
and green/red editor gutters as a native "Run with Coverage" session.

## Viewing coverage in CLion

CLion has a built-in coverage runner that works without `lcov`/`genhtml` —
useful on any platform, including Windows, as long as the CMake profile is
configured with a GCC-based toolchain (e.g. MinGW-w64). Plain MSVC builds
get no `--coverage` instrumentation at all (see the Windows note above),
so CLion has nothing to collect there either.

1. **Set up a Test CMake profile** (if you don't have one): **Settings →
   Build, Execution, Deployment → CMake** → add a profile with **Build
   type** = `Test`. Reuse the vcpkg toolchain setup already described in
   [COMPILE.md](./COMPILE.md)'s "IDE Setup" section (CMake preset or
   `-DCMAKE_TOOLCHAIN_FILE=...`) so the profile can actually configure.
2. **Run with coverage**: in the CMake/Test tool window (or the class
   gutter icons inside a `*Tests.cpp` file), right-click
   `Adventure_Designer_Studio_Tests` (or an individual `TEST`/`TEST_F`) and
   choose **"Run … with Coverage"** — or use the coverage icon in the
   toolbar next to Run/Debug (two overlapping bars).
3. CLion instruments the binary with `gcov` itself (it picks up the
   `--coverage` flags already baked into `CMAKE_CXX_FLAGS_TEST` — no extra
   CMake configuration needed) and opens a **Coverage** tool window: a
   file tree with per-file/per-directory percentages, and green/red line
   gutters directly in the editor showing exactly which lines ran.

   > **This is why the files seemed to be "missing":** by default CLion
   > keeps this run's `.coverage` snapshot in its own IDE-managed cache
   > (outside the project directory), not as project files. Nothing shows
   > up under `tests/coverage/` until you explicitly export it — step 4.
4. To save a copy in the project, right-click the report root in the
   Coverage tool window → **"Generate Coverage Report..."** → choose
   **HTML report** and point the output directory at
   `tests/coverage/reports/html-clion/` (keep it under
   `tests/coverage/reports/`, alongside the `coverage.sh` output, since
   that whole folder is already gitignored as generated content).

**Excluding `*Tests.cpp` from the report:** CLion's C/C++ coverage runner
(gcov/llvm-cov based) has no built-in file-exclusion setting — the
"Excluded classes and files" pattern list some JetBrains IDEs expose is a
JVM (Java/Kotlin) coverage feature and doesn't apply here. The Coverage
tool window and any report exported from it (step 4 above) will include
`*Tests.cpp` files. If that's a problem, use `coverage.sh`'s `lcov
--remove` instead (already configured to strip `*/tests/*Tests.cpp` — see
above), which produces a `coverage.info` you can [import back into
CLion](#importing-coverages-report-into-clion) already filtered.

## Viewing coverage in VS Code (MinGW GCC only)

VS Code has no built-in coverage runner. On Windows, with a **MinGW-w64
GCC** toolchain (MSVC produces no `--coverage` data — see the Windows note
above), use `gcovr` instead of `lcov`/`genhtml`: it's pure Python, reads
the same `.gcda`/`.gcno` files directly, and needs no bash or Perl.

**Requires `gcovr`:**

```bash
pip install gcovr
```

**Configure and build the `Test` type, then run it from the build directory:**

```bash
cmake -B build-coverage -DCMAKE_BUILD_TYPE=Test
cmake --build build-coverage
ctest --test-dir build-coverage --output-on-failure
```

**Generate an HTML report directly:**

```bash
gcovr --root . --object-directory build-coverage \
    --exclude 'lib/.*' --exclude 'tests/.*' --exclude '.*/vcpkg_installed/.*' \
    --html --html-details -o tests/coverage/reports/html-gcovr/index.html
```

**Or emit an lcov-format tracefile** for the [Coverage
Gutters](https://marketplace.visualstudio.com/items?itemName=ryanluker.vscode-coverage-gutters)
extension, which then renders inline green/red gutters directly in the
editor:

```bash
gcovr --root . --object-directory build-coverage \
    --exclude 'lib/.*' --exclude 'tests/.*' --exclude '.*/vcpkg_installed/.*' \
    --lcov -o tests/coverage/reports/coverage.info
```

Then in VS Code: install Coverage Gutters, run **"Coverage Gutters: Display
Coverage"** (or **"Watch"**) from the command palette, and point it at
`tests/coverage/reports/coverage.info` if it isn't picked up automatically.

## Troubleshooting

**`ctest --test-dir build` reports "No tests were found"**

The build wasn't configured as `Test`. Reconfigure with
`./build.sh Test` (or `build.bat Test`) — `Debug`/`Release` builds don't
compile `tests/`.

**`lcov: command not found` / `genhtml: command not found`**

Install `lcov` (see Prerequisites above) — it bundles `genhtml`. If you
don't want the extra dependency, use CLion's built-in coverage runner
instead (see above), which doesn't need either tool.

**`coverage.sh` runs the whole build+test suite, then exits with an lcov
`ERROR: (inconsistent)` or `ERROR: (unused)` and no report appears**

A known `lcov` 2.x compatibility issue, not a bug in the test suite itself:
newer `gcov` (GCC 15+) intermediate-format output trips lcov 2.x's
stricter per-function consistency checks, and a fresh checkout may exclude
a pattern (`/usr/*`, etc.) that ends up matching zero remaining files after
capture — lcov 2.x treats *both* as hard errors by default, and
`set -e` kills the script right there, **before `genhtml` ever runs**. From
the outside this looks like "the report disappeared when the script
finished" — really it was never written. `tests/coverage/coverage.sh`
already passes `--ignore-errors inconsistent,unused` to work around this;
if you still hit it (e.g. an older `lcov`, or a different distro's error
set), add the specific error category lcov names in its own message to
the `--ignore-errors` list on the failing call.

**CLion's Coverage tool window opens but shows 0% / no data for every file**

The active CMake profile's **Build type** must be `Test` — not just be
*named* "Test"/"Tests". `--coverage` instrumentation only gets added when
`CMAKE_BUILD_TYPE` is literally `Test` (see `CMAKE_CXX_FLAGS_TEST` in the
root `CMakeLists.txt`); a profile named "Tests" that's actually configured
with Build type `Debug` builds an entirely uninstrumented binary, so CLion
has nothing to report. Check **Settings → Build, Execution, Deployment →
CMake**, select the profile you're using to run tests, and confirm its
**Build type** dropdown reads `Test`. If it doesn't, change it (or clone a
new profile with Build type `Test`), reload the CMake project, and re-run
**"Run … with Coverage"**.

**Coverage numbers look stale after re-running the script**

`tests/coverage/coverage.sh` reuses `build-coverage/`. Delete it along with
the generated reports (`rm -rf build-coverage tests/coverage/reports`) and
re-run if `.gcda` files from a previous run seem to be mixing with a changed
test binary (gcov warns `overwriting an existing profile data with a
different checksum` in this case — harmless, but a clean rebuild avoids the
noise).

**CLion coverage run finished but I can't find any files**

Expected — see "This is why the files seemed to be missing" above. CLion's
coverage results live in its own cache until you explicitly export them via
**"Generate Coverage Report..."** in the Coverage tool window.

**New test file isn't being picked up**

Confirm it matches `tests/**/*.cpp` and re-run CMake configure
(`cmake --build build --target Adventure_Designer_Studio_Tests`) — the glob
is evaluated at configure time, so a brand-new file needs a fresh
`cmake -B build ...` (or just re-run `./build.sh Test`) before it's
compiled in.
