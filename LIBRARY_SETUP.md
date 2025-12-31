# Library Setup Guide

## Overview

Adventure Designer Studio uses a hybrid approach for managing libraries:

## 1. Build Dependencies (via vcpkg)

These are automatically downloaded and built by vcpkg:

- **nlohmann-json** - JSON library
- **spdlog** - Logging library
- **boost-uuid** - UUID generation
- **imgui** - GUI library (with SDL2 bindings)
- **SDL2** - Graphics and input
- **gtest** - Testing framework

✅ **No manual setup needed** - just run `./build.sh` and vcpkg handles everything!

## 2. Git Submodule (tracked in repository)

- **lib/IconFontCppHeaders/** - Header-only library for icon fonts

✅ **Already in repository** - initialized automatically when you clone

## 3. Reference Libraries (optional, for code browsing)

These are **NOT tracked in git** but you can clone them locally to browse their source code in your IDE:

- **lib/imgui/**
- **lib/nlohmann_json/**
- **lib/spdlog/**

### Why keep reference libraries?

While vcpkg manages the actual build, having the source code locally allows you to:
- Browse the library implementation
- Set breakpoints in library code while debugging
- Understand how the libraries work internally
- Jump to definitions in your IDE

### How to setup reference libraries:

```bash
./setup-reference-libs.sh
```

This script clones the latest versions to your local `lib/` directory.

### Important Notes:

⚠️ **The build NEVER uses these local copies!**
- Build uses only vcpkg-managed versions
- These are purely for IDE code navigation
- Safe to delete/re-clone anytime
- Not committed to git (in .gitignore)

## New Developer Setup

When you clone this repository:

1. **Install system build tools** (one-time):
   ```bash
   # Linux (Debian/Ubuntu)
   sudo apt install build-essential autoconf automake libtool pkg-config python3-jinja2

   # Linux (Fedora/RHEL)
   sudo dnf install gcc-c++ autoconf automake libtool pkgconfig python3-jinja2

   # macOS
   brew install autoconf automake libtool pkg-config
   pip3 install jinja2
   ```

2. **Install vcpkg** (one-time):
   ```bash
   # Clone vcpkg to /opt (system-wide)
   sudo git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg
   sudo chown -R $USER:$USER /opt/vcpkg
   cd /opt/vcpkg
   ./bootstrap-vcpkg.sh
   export VCPKG_ROOT=/opt/vcpkg
   echo 'export VCPKG_ROOT=/opt/vcpkg' >> ~/.bashrc
   ```

3. **(Optional) Clone reference libraries**:
   ```bash
   ./setup-reference-libs.sh
   ```

4. **Build the project**:
   ```bash
   ./build.sh
   ```

That's it! The first build takes longer as vcpkg compiles dependencies.

## FAQ

**Q: Do I need to run setup-reference-libs.sh?**
A: No, it's optional. Only if you want to browse library source code in your IDE.

**Q: Will the reference libraries affect my build?**
A: No, the build only uses vcpkg versions. CMakeLists.txt doesn't reference lib/ anymore.

**Q: Can I delete lib/imgui if I don't need it?**
A: Yes! It's safe to delete anytime. Just re-run `./setup-reference-libs.sh` if you want it back.

**Q: What if reference libraries get out of sync with vcpkg versions?**
A: That's fine - they're just for browsing. If you want to sync them, delete and re-run the setup script.

**Q: Why keep IconFontCppHeaders as a submodule?**
A: It's header-only and not available in vcpkg, so we include it directly in the repo.

---

For more details, see [VCPKG_MIGRATION.md](VCPKG_MIGRATION.md)
