#!/usr/bin/env bash
#
# Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
#
# compile-osx.sh — one-shot build & delivery script for Adventure Designer Studio
#                  on macOS (Apple Silicon and Intel).
#
# It takes a near-clean Mac to a ready-to-run artifact:
#   1. detects the environment (arch, CPU count)
#   2. checks the toolchain and installs whatever is missing
#      (Xcode CLT, Homebrew, CMake, git, Ninja, vcpkg build deps)
#   3. finds an existing vcpkg or clones + bootstraps one into ~/vcpkg
#   4. initialises the IconFontCppHeaders submodule
#   5. configures and builds the project through the vcpkg toolchain
#   6. stages a .app bundle AND a plain run folder into ./dist
#
# NOTE ON PORTABILITY: the project's CMakeLists.txt forces `-march=native` for
# Clang, so the produced binary is tuned for THIS Mac's CPU. Treat dist/ as
# "runs on this machine (and CPU-compatible siblings)", not a universal build.
#
# Usage:
#   ./compile-osx.sh [BUILD_TYPE] [--no-install] [--clean] [--no-package] [-h]
#
#     BUILD_TYPE     Release (default) | Debug | Test
#     --no-install   only check prerequisites; never call brew / xcode-select.
#                    Abort with instructions on the first gap.
#     --clean        remove the build directory before configuring.
#     --no-package   configure + build only; skip the dist/ staging step.
#     -h, --help     show this help and exit.

set -euo pipefail

# ---------------------------------------------------------------------------
# --- Constants
# ---------------------------------------------------------------------------
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${REPO_ROOT}/build"
DIST_DIR="${REPO_ROOT}/dist"
BIN_NAME="Adventure_Designer_Studio"
APP_NAME="Adventure Designer Studio"
BUNDLE_ID="com.chernandezosma.adventure-designer-studio"
VCPKG_REPO="https://github.com/microsoft/vcpkg.git"
BREW_INSTALL_URL="https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh"

START_TS=$(date +%s)

# ---------------------------------------------------------------------------
# --- Output helpers
# ---------------------------------------------------------------------------
if [ -t 1 ]; then
    C_RESET=$'\033[0m'; C_BOLD=$'\033[1m'; C_BLUE=$'\033[34m'
    C_GREEN=$'\033[32m'; C_YELLOW=$'\033[33m'; C_RED=$'\033[31m'
else
    C_RESET=""; C_BOLD=""; C_BLUE=""; C_GREEN=""; C_YELLOW=""; C_RED=""
fi

step()  { printf '\n%s==>%s %s%s\n' "${C_BLUE}${C_BOLD}" "${C_RESET}${C_BOLD}" "$*" "${C_RESET}"; }
info()  { printf '    %s\n' "$*"; }
ok()    { printf '    %s✓%s %s\n' "${C_GREEN}" "${C_RESET}" "$*"; }
warn()  { printf '    %s!%s %s\n' "${C_YELLOW}" "${C_RESET}" "$*" >&2; }
die()   { printf '\n%sERROR:%s %s\n' "${C_RED}${C_BOLD}" "${C_RESET}" "$*" >&2; exit 1; }

# ---------------------------------------------------------------------------
# --- Argument parsing
# ---------------------------------------------------------------------------
BUILD_TYPE="Release"
DO_INSTALL=1
DO_CLEAN=0
DO_PACKAGE=1

print_usage() { sed -n '2,40p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'; }

for arg in "$@"; do
    case "$arg" in
        Release|Debug|Test)  BUILD_TYPE="$arg" ;;
        --no-install)        DO_INSTALL=0 ;;
        --clean)             DO_CLEAN=1 ;;
        --no-package)        DO_PACKAGE=0 ;;
        -h|--help)           print_usage; exit 0 ;;
        *)                   die "Unknown argument: '$arg' (try --help)" ;;
    esac
done

# ---------------------------------------------------------------------------
# --- Environment detection
# ---------------------------------------------------------------------------
step "Detecting environment"

[ "$(uname -s)" = "Darwin" ] || die "This script is for macOS. Use compile-nix.sh on Linux."

ARCH="$(uname -m)"                                   # arm64 | x86_64
NPROC=$( (command -v sysctl >/dev/null && sysctl -n hw.ncpu) || echo 4 )
MACOS_VER="$(sw_vers -productVersion 2>/dev/null || echo '?')"

info "macOS      : ${MACOS_VER}"
info "Arch       : ${ARCH}"
info "CPUs       : ${NPROC}"
info "Build type : ${BUILD_TYPE}"

confirm() {
    # $1 = prompt; returns 0 on yes. Non-interactive => yes.
    [ -t 0 ] || return 0
    read -r -p "    $1 [Y/n] " reply
    case "$reply" in [nN]*) return 1 ;; *) return 0 ;; esac
}

# ---------------------------------------------------------------------------
# --- Xcode Command Line Tools (provides clang, make, git)
# ---------------------------------------------------------------------------
step "Checking Xcode Command Line Tools"
if ! xcode-select -p >/dev/null 2>&1; then
    [ "$DO_INSTALL" -eq 1 ] || die "Xcode Command Line Tools missing. Install with:
    xcode-select --install"
    warn "Xcode Command Line Tools are required (clang / git / make)."
    if confirm "Run 'xcode-select --install' now?"; then
        xcode-select --install || true
        info "A macOS dialog should have opened. Finish that install, then re-run this script."
        die "Waiting on Xcode Command Line Tools installation."
    else
        die "Aborted — install the Command Line Tools and re-run."
    fi
fi
ok "Command Line Tools: $(xcode-select -p)"

# ---------------------------------------------------------------------------
# --- Homebrew
# ---------------------------------------------------------------------------
step "Checking Homebrew"
if ! command -v brew >/dev/null 2>&1; then
    # Common non-PATH locations
    for b in /opt/homebrew/bin/brew /usr/local/bin/brew; do
        [ -x "$b" ] && eval "$("$b" shellenv)" && break
    done
fi
if ! command -v brew >/dev/null 2>&1; then
    [ "$DO_INSTALL" -eq 1 ] || die "Homebrew missing. Install from https://brew.sh then re-run."
    warn "Homebrew is the package source for CMake / Ninja / autotools."
    if confirm "Install Homebrew now (official installer)?"; then
        /bin/bash -c "$(curl -fsSL "$BREW_INSTALL_URL")"
        for b in /opt/homebrew/bin/brew /usr/local/bin/brew; do
            [ -x "$b" ] && eval "$("$b" shellenv)" && break
        done
        command -v brew >/dev/null 2>&1 || die "Homebrew install did not put 'brew' on PATH.
    Open a new terminal (or run: eval \"\$(/opt/homebrew/bin/brew shellenv)\") and re-run."
    else
        die "Aborted — install Homebrew and re-run."
    fi
fi
ok "Homebrew $(brew --version | head -1 | awk '{print $2}')"

# $1 = label, rest = brew formula names
brew_install() {
    local label="$1"; shift
    if [ "$DO_INSTALL" -eq 0 ]; then
        die "$label missing and --no-install was given. Install manually:
    brew install $*"
    fi
    warn "Installing ${label}: $*"
    confirm "Run 'brew install $*'?" || die "Aborted by user."
    brew install "$@"
}

# ---------------------------------------------------------------------------
# --- CMake, git, Ninja, autotools
# ---------------------------------------------------------------------------
step "Checking build toolchain"

# --- CMake (>= 3.21) --------------------------------------------------
need_cmake=0
if command -v cmake >/dev/null 2>&1; then
    CM_VER="$(cmake --version | head -1 | grep -oE '[0-9]+\.[0-9]+' | head -1)"
    CM_MAJOR="${CM_VER%%.*}"; CM_MINOR="${CM_VER##*.}"
    if [ "$CM_MAJOR" -lt 3 ] || { [ "$CM_MAJOR" -eq 3 ] && [ "$CM_MINOR" -lt 21 ]; }; then
        warn "CMake ${CM_VER} too old (need >= 3.21)"; need_cmake=1
    else
        ok "CMake ${CM_VER}"
    fi
else
    need_cmake=1
fi
[ "$need_cmake" -eq 1 ] && { brew_install "CMake" cmake; ok "CMake $(cmake --version | head -1 | awk '{print $3}')"; }

# --- git ------------------------------------------------------------
command -v git >/dev/null 2>&1 || brew_install "git" git
ok "git $(git --version | awk '{print $3}')"

# --- Ninja (preferred generator) ----------------------------------
GENERATOR_ARGS=()
if command -v ninja >/dev/null 2>&1; then
    ok "Ninja $(ninja --version)"
    GENERATOR_ARGS=(-G Ninja)
elif [ "$DO_INSTALL" -eq 1 ]; then
    brew_install "Ninja (optional)" ninja || true
    if command -v ninja >/dev/null 2>&1; then
        ok "Ninja $(ninja --version)"; GENERATOR_ARGS=(-G Ninja)
    else
        warn "Ninja unavailable; using the default Make generator."
    fi
else
    warn "Ninja not found; using the default Make generator."
fi

# --- autotools + pkg-config (needed by some vcpkg ports) ---------
missing_autotools=()
for t in autoconf automake libtool pkg-config; do
    command -v "$t" >/dev/null 2>&1 || missing_autotools+=("$t")
done
if [ "${#missing_autotools[@]}" -gt 0 ]; then
    brew_install "autotools" autoconf autoconf-archive automake libtool pkg-config
else
    ok "autoconf / automake / libtool / pkg-config present"
fi

# --- python3 + jinja2 (wayland-protocols style codegen) ----------
if command -v python3 >/dev/null 2>&1; then
    if ! python3 -c 'import jinja2' >/dev/null 2>&1; then
        if [ "$DO_INSTALL" -eq 1 ]; then
            python3 -m pip install --user jinja2 >/dev/null 2>&1 || true
        fi
        python3 -c 'import jinja2' >/dev/null 2>&1 \
            && ok "python3 + jinja2 present" \
            || warn "python3 'jinja2' not importable — 'python3 -m pip install --user jinja2' if a port fails"
    else
        ok "python3 + jinja2 present"
    fi
else
    warn "python3 not found (usually provided by the Command Line Tools)"
fi

# ---------------------------------------------------------------------------
# --- vcpkg
# ---------------------------------------------------------------------------
step "Locating vcpkg"

vcpkg_valid() { [ -n "${1:-}" ] && [ -f "$1/scripts/buildsystems/vcpkg.cmake" ]; }

VCPKG_DIR=""
if vcpkg_valid "${VCPKG_ROOT:-}"; then
    VCPKG_DIR="$VCPKG_ROOT"
    ok "Using \$VCPKG_ROOT: ${VCPKG_DIR}"
else
    for cand in "${HOME}/vcpkg" "/opt/vcpkg" "/usr/local/vcpkg" "${HOME}/src/vcpkg"; do
        if vcpkg_valid "$cand"; then VCPKG_DIR="$cand"; ok "Found vcpkg at ${VCPKG_DIR}"; break; fi
    done
fi

if [ -z "$VCPKG_DIR" ]; then
    TARGET="${HOME}/vcpkg"
    step "Cloning vcpkg into ${TARGET}"
    [ "$DO_INSTALL" -eq 1 ] || die "vcpkg not found and --no-install was given.
    git clone ${VCPKG_REPO} ${TARGET} && ${TARGET}/bootstrap-vcpkg.sh"
    if [ -d "$TARGET/.git" ]; then
        git -C "$TARGET" pull --ff-only || warn "git pull failed; using existing checkout"
    else
        git clone "$VCPKG_REPO" "$TARGET"
    fi
    "$TARGET/bootstrap-vcpkg.sh"
    vcpkg_valid "$TARGET" || die "vcpkg bootstrap did not produce the toolchain file."
    VCPKG_DIR="$TARGET"
    ok "vcpkg ready at ${VCPKG_DIR}"
fi

export VCPKG_ROOT="$VCPKG_DIR"
grep -qs "VCPKG_ROOT=" "${HOME}/.zshrc" "${HOME}/.bashrc" 2>/dev/null \
    || info "Tip: add 'export VCPKG_ROOT=\"${VCPKG_DIR}\"' to your ~/.zshrc to persist it."

# ---------------------------------------------------------------------------
# --- Submodules
# ---------------------------------------------------------------------------
step "Initialising submodules"
if [ -e "${REPO_ROOT}/.git" ]; then
    git -C "$REPO_ROOT" submodule update --init --recursive
    ok "submodules up to date"
elif [ -f "${REPO_ROOT}/lib/IconFontCppHeaders/IconsFontAwesome6.h" ]; then
    warn "Not a git checkout; assuming vendored lib/IconFontCppHeaders is present."
else
    die "Not a git checkout and lib/IconFontCppHeaders is missing.
    Clone the repo with git instead of downloading a zip."
fi

# ---------------------------------------------------------------------------
# --- Configure
# ---------------------------------------------------------------------------
step "Configuring (${BUILD_TYPE})"

if [ "$DO_CLEAN" -eq 1 ] && [ -d "$BUILD_DIR" ]; then
    info "Removing ${BUILD_DIR}"; rm -rf "$BUILD_DIR"
fi

# A pre-existing cache built with a different generator makes CMake refuse to
# reconfigure — wipe it rather than fail.
if [ -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    want_gen="Unix Makefiles"
    [ "${#GENERATOR_ARGS[@]}" -gt 0 ] && want_gen="Ninja"
    have_gen="$(sed -n 's/^CMAKE_GENERATOR:INTERNAL=//p' "${BUILD_DIR}/CMakeCache.txt")"
    if [ -n "$have_gen" ] && [ "$have_gen" != "$want_gen" ]; then
        warn "Existing build/ uses generator '${have_gen}', need '${want_gen}' — wiping build/"
        rm -rf "$BUILD_DIR"
    fi
fi
mkdir -p "$BUILD_DIR"

cmake -S "$REPO_ROOT" -B "$BUILD_DIR" "${GENERATOR_ARGS[@]}" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_TOOLCHAIN_FILE="${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake" \
    -DVCPKG_INSTALLED_DIR="${BUILD_DIR}/vcpkg_installed" \
    -DVCPKG_OVERLAY_PORTS="${REPO_ROOT}/vcpkg-overlays"

# ---------------------------------------------------------------------------
# --- Build
# ---------------------------------------------------------------------------
step "Building (first run downloads + compiles all vcpkg deps — 10-20 min)"
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -j "$NPROC"

# ---------------------------------------------------------------------------
# --- Locate the produced binary
# ---------------------------------------------------------------------------
BIN_PATH=""
for cand in "${BUILD_DIR}/${BIN_NAME}" "${BUILD_DIR}/${BUILD_TYPE}/${BIN_NAME}"; do
    [ -x "$cand" ] && { BIN_PATH="$cand"; break; }
done
[ -n "$BIN_PATH" ] || BIN_PATH="$(find "$BUILD_DIR" -maxdepth 3 -type f -name "$BIN_NAME" -perm -u+x 2>/dev/null | head -1)"
[ -n "$BIN_PATH" ] && [ -x "$BIN_PATH" ] || die "Build finished but '${BIN_NAME}' was not found under ${BUILD_DIR}."
ok "Binary: ${BIN_PATH}"

if [ "$DO_PACKAGE" -eq 0 ]; then
    step "Done (--no-package: skipped staging)"
    info "Run it with:  cd '$(dirname "$BIN_PATH")' && ./${BIN_NAME}"
    exit 0
fi

# ---------------------------------------------------------------------------
# --- Stage into dist/  (a .app bundle + a plain run folder)
# ---------------------------------------------------------------------------
step "Staging runnable artifact into ${DIST_DIR}"

rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

VERSION="$(grep -oE '"version"[[:space:]]*:[[:space:]]*"[^"]+"' "${REPO_ROOT}/vcpkg.json" | grep -oE '[0-9][^"]*' | head -1)"
[ -n "$VERSION" ] || VERSION="0.1.0"

ENV_SRC=""
if [ -f "${REPO_ROOT}/.env" ]; then ENV_SRC="${REPO_ROOT}/.env"
elif [ -f "${REPO_ROOT}/.env.example" ]; then ENV_SRC="${REPO_ROOT}/.env.example"; warn ".env not found — using .env.example"
fi

# --- 1. Plain run folder -------------------------------------------
install -m 0755 "$BIN_PATH" "${DIST_DIR}/${BIN_NAME}"
cp -R "${REPO_ROOT}/public" "${DIST_DIR}/public"
[ -n "$ENV_SRC" ] && cp "$ENV_SRC" "${DIST_DIR}/.env"

cat > "${DIST_DIR}/run.command" <<EOF
#!/usr/bin/env bash
here="\$(cd "\$(dirname "\${BASH_SOURCE[0]}")" && pwd)"
cd "\$here"
[ -d "\$here/libs" ] && export DYLD_LIBRARY_PATH="\$here/libs:\${DYLD_LIBRARY_PATH:-}"
exec "\$here/${BIN_NAME}" "\$@"
EOF
chmod +x "${DIST_DIR}/run.command"

# --- 2. .app bundle ---------------------------------------------
APP_DIR="${DIST_DIR}/${APP_NAME}.app"
MACOS_DIR="${APP_DIR}/Contents/MacOS"
RES_DIR="${APP_DIR}/Contents/Resources"
mkdir -p "$MACOS_DIR" "$RES_DIR"

install -m 0755 "$BIN_PATH" "${MACOS_DIR}/${BIN_NAME}"
cp -R "${REPO_ROOT}/public" "${MACOS_DIR}/public"
[ -n "$ENV_SRC" ] && cp "$ENV_SRC" "${MACOS_DIR}/.env"

if [ -f "${REPO_ROOT}/public/assets/icon.bmp" ]; then
    cp "${REPO_ROOT}/public/assets/icon.bmp" "${RES_DIR}/icon.bmp"
fi

cat > "${APP_DIR}/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>                <string>${APP_NAME}</string>
    <key>CFBundleDisplayName</key>         <string>${APP_NAME}</string>
    <key>CFBundleIdentifier</key>          <string>${BUNDLE_ID}</string>
    <key>CFBundleVersion</key>             <string>${VERSION}</string>
    <key>CFBundleShortVersionString</key>  <string>${VERSION}</string>
    <key>CFBundlePackageType</key>         <string>APPL</string>
    <key>CFBundleExecutable</key>          <string>${BIN_NAME}</string>
    <key>LSMinimumSystemVersion</key>      <string>11.0</string>
    <key>NSHighResolutionCapable</key>     <true/>
</dict>
</plist>
EOF

# Wrapper so the bundled binary runs with Contents/MacOS as CWD (finds public/ + .env).
mv "${MACOS_DIR}/${BIN_NAME}" "${MACOS_DIR}/${BIN_NAME}.bin"
cat > "${MACOS_DIR}/${BIN_NAME}" <<EOF
#!/bin/bash
d="\$(cd "\$(dirname "\$0")" && pwd)"
cd "\$d"
[ -d "\$d/libs" ] && export DYLD_LIBRARY_PATH="\$d/libs:\${DYLD_LIBRARY_PATH:-}"
exec "\$d/${BIN_NAME}.bin" "\$@"
EOF
chmod +x "${MACOS_DIR}/${BIN_NAME}"

# --- 3. Bundle non-system dylibs ------------------------------
if command -v dylibbundler >/dev/null 2>&1; then
    info "Running dylibbundler on the bundle binary"
    dylibbundler -of -cd -b \
        -x "${MACOS_DIR}/${BIN_NAME}.bin" \
        -d "${MACOS_DIR}/libs" \
        -p "@executable_path/libs" || warn "dylibbundler reported problems; check output above"
    # mirror libs into the plain folder too
    [ -d "${MACOS_DIR}/libs" ] && cp -R "${MACOS_DIR}/libs" "${DIST_DIR}/libs"
else
    warn "dylibbundler not installed — copying vcpkg dylibs by hand (best effort)."
    warn "For a cleaner bundle: brew install dylibbundler, then re-run."
    mkdir -p "${MACOS_DIR}/libs"
    copied=0
    while read -r line; do
        lib="$(printf '%s' "$line" | awk '{print $1}')"
        case "$lib" in
            "${BUILD_DIR}/vcpkg_installed"/*|*/vcpkg_installed/*)
                [ -f "$lib" ] && { cp -u "$lib" "${MACOS_DIR}/libs/"; copied=$((copied+1)); } ;;
        esac
    done < <(otool -L "${MACOS_DIR}/${BIN_NAME}.bin" | tail -n +2)
    if [ "$copied" -gt 0 ]; then
        cp -R "${MACOS_DIR}/libs" "${DIST_DIR}/libs"
        ok "copied ${copied} dylib(s)"
    else
        rmdir "${MACOS_DIR}/libs" 2>/dev/null || true
    fi
fi

# --- 4. Ad-hoc codesign so Gatekeeper allows a local run -----
if command -v codesign >/dev/null 2>&1; then
    codesign --force --deep --sign - "$APP_DIR" >/dev/null 2>&1 \
        && ok "ad-hoc codesigned ${APP_NAME}.app" \
        || warn "codesign failed; on first launch right-click the app → Open"
fi

# ---------------------------------------------------------------------------
# --- Verify
# ---------------------------------------------------------------------------
step "Verifying artifact"
unresolved="$(otool -L "${DIST_DIR}/${BIN_NAME}" 2>/dev/null | tail -n +2 | awk '{print $1}' \
    | while read -r l; do
        case "$l" in
            /usr/lib/*|/System/*|@rpath/*|@executable_path/*|@loader_path/*) ;;
            *) [ -f "$l" ] || echo "$l" ;;
        esac
      done)"
if [ -n "$unresolved" ]; then
    warn "dylibs that may not exist on another Mac:"
    printf '%s\n' "$unresolved" | sed 's/^/        /'
    warn "Install dylibbundler and re-run for a self-contained bundle."
else
    ok "all linked dylibs are system libs or resolvable"
fi
[ -f "${DIST_DIR}/public/translations/core/en_US.json" ] \
    && ok "assets staged" \
    || warn "public/translations/core/en_US.json missing — check public/ contents"

# ---------------------------------------------------------------------------
# --- Summary
# ---------------------------------------------------------------------------
ELAPSED=$(( $(date +%s) - START_TS ))
step "Build complete in $((ELAPSED / 60))m $((ELAPSED % 60))s"
cat <<EOF
    Build type : ${BUILD_TYPE}
    vcpkg root : ${VCPKG_DIR}
    Artifact   : ${DIST_DIR}
                   • ${APP_NAME}.app   (double-click / drag to /Applications)
                   • ${BIN_NAME} + run.command   (plain folder)

    Run it:
      open "${DIST_DIR}/${APP_NAME}.app"

    Ship it:
      ditto -c -k --keepParent "${DIST_DIR}/${APP_NAME}.app" ADS-macos-${ARCH}.zip

    Reminder: built with -march=native — targets this Mac's CPU family.
EOF
