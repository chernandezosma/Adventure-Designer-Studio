# vcpkg Migration Guide

## What Changed

The Adventure Designer Studio project has been migrated from git submodules to **vcpkg** for dependency management. This provides:

- ✅ **Cross-platform compatibility** (Linux, Windows, macOS)
- ✅ **Automatic dependency resolution**
- ✅ **Version pinning** for reproducible builds
- ✅ **Simplified dependency management**

## Dependencies Migrated

The following dependencies are now managed by vcpkg:

- **nlohmann-json** (JSON library)
- **spdlog** (Logging library)
- **boost-uuid** (UUID generation - replaced crashoz/uuid_v4)
- **imgui** (with SDL2 bindings and docking)
- **SDL2** (Graphics and input)
- **gtest** (Testing framework)

**Note:** `IconFontCppHeaders` remains as a git submodule since it's header-only.

## Library Organization

The project now has two types of libraries:

### In Git Repository:
- **lib/IconFontCppHeaders/** - Git submodule (header-only, still needed)

### Local Reference Only (NOT in git):
- **lib/imgui/** - For code browsing/reference
- **lib/nlohmann_json/** - For code browsing/reference
- **lib/spdlog/** - For code browsing/reference

These reference libraries are ignored by git but you can clone them locally using:
```bash
./setup-reference-libs.sh
```

**Important:** The build uses vcpkg versions, NOT these local copies!

## First-Time Setup

### 1. Install System Build Tools

These are required by vcpkg to build certain dependencies:

**Linux (Debian/Ubuntu):**
```bash
sudo apt install build-essential autoconf automake libtool pkg-config python3-jinja2
```

**Linux (Fedora/RHEL):**
```bash
sudo dnf install gcc-c++ autoconf automake libtool pkgconfig python3-jinja2
```

**macOS:**
```bash
brew install autoconf automake libtool pkg-config
pip3 install jinja2
```

**Windows:**
Most tools are included with Visual Studio Build Tools. No additional installation needed.

### 2. Install vcpkg

Choose your operating system:

**Linux/Mac:**
```bash
# Clone vcpkg to /opt (system-wide installation)
sudo git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg

# Change ownership to your user (so you don't need sudo for vcpkg commands)
sudo chown -R $USER:$USER /opt/vcpkg

# Bootstrap vcpkg
cd /opt/vcpkg
./bootstrap-vcpkg.sh

# Set environment variable
export VCPKG_ROOT=/opt/vcpkg
```

Add to your `.bashrc` or `.zshrc` for persistence:
```bash
echo 'export VCPKG_ROOT=/opt/vcpkg' >> ~/.bashrc
source ~/.bashrc
```

**Windows:**
```powershell
# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat

# Set environment variable (persistent)
setx VCPKG_ROOT "C:\vcpkg"
```

### 3. (Optional) Setup Reference Libraries

For code browsing and reference (not required for building):
```bash
./setup-reference-libs.sh
```

This clones imgui, nlohmann_json, and spdlog to your local `lib/` folder so you can browse their source code in your IDE.

### 4. Build the Project

**Linux/Mac:**
```bash
./build.sh          # Release build (default)
./build.sh Debug    # Debug build
./build.sh Test     # Test build
```

**Windows:**
```batch
build.bat           REM Release build (default)
build.bat Debug     REM Debug build
build.bat Test      REM Test build
```

The first build will take longer as vcpkg downloads and compiles all dependencies. Subsequent builds will be much faster.

## Code Changes

### UUID Migration: uuid_v4 → boost::uuid

**Old code (uuid_v4):**
```cpp
#include "uuid_v4.h"

UUIDv4::UUID uuid = getRandomUuid();
UUIDv4::UUIDGenerator<std::mt19937_64> gen;
```

**New code (boost::uuid):**
```cpp
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

boost::uuids::uuid uuid = getRandomUuid();
boost::uuids::random_generator gen;
```

**For unordered_map:**
```cpp
#include <boost/functional/hash.hpp>

std::unordered_map<boost::uuids::uuid, Window*, boost::hash<boost::uuids::uuid>> windows;
```

## Troubleshooting

### "Cannot find vcpkg"

Make sure `VCPKG_ROOT` is set:
```bash
echo $VCPKG_ROOT  # Linux/Mac
echo %VCPKG_ROOT%  # Windows
```

### Compilation Errors

1. Delete the `build/` directory
2. Run the build script again to reconfigure from scratch

### vcpkg Download Issues

If vcpkg fails to download packages:
```bash
cd $VCPKG_ROOT
git pull
./vcpkg update
```

## File Structure

```
Adventure-Designer-Studio/
├── vcpkg.json                  # Dependency manifest
├── vcpkg-configuration.json    # vcpkg configuration
├── CMakeLists.txt             # Updated for vcpkg
├── build.sh                   # Linux/Mac build script
├── build.bat                  # Windows build script
├── setup-reference-libs.sh    # Setup reference libraries for browsing
├── build/                     # Build output (gitignored)
│   └── vcpkg_installed/      # Installed dependencies (gitignored)
└── lib/
    ├── IconFontCppHeaders/   # Git submodule (tracked)
    ├── imgui/                # Local reference only (gitignored)
    ├── nlohmann_json/        # Local reference only (gitignored)
    └── spdlog/               # Local reference only (gitignored)
```

## Benefits of vcpkg

1. **Version Pinning**: The `builtin-baseline` in `vcpkg.json` ensures everyone builds with the same library versions
2. **No More Submodule Hell**: No need to manage git submodules manually
3. **True Cross-Platform**: Same build process works on Windows, Linux, and macOS
4. **CMake Integration**: Automatic find_package() support
5. **Binary Caching**: vcpkg can cache compiled libraries for faster rebuilds

## Baseline Version

Current vcpkg baseline: **84bab45af87c951d213f093450b1a1ec04777b93** (2025.12.12 release)

## References

- [vcpkg Documentation](https://vcpkg.io/)
- [vcpkg GitHub](https://github.com/microsoft/vcpkg)
- [Boost.UUID Documentation](https://www.boost.org/doc/libs/release/libs/uuid/)

---

**Migration completed**: December 30, 2025
