#!/usr/bin/env bash
#
# Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
#
# compile-nix.sh — one-shot build & delivery script for Adventure Designer Studio
#                  on Linux (Debian/Ubuntu and Fedora/RHEL families).
#
# It takes a near-clean machine to a ready-to-run artifact:
#   1. detects the environment (distro family, arch, CPU count)
#   2. checks the toolchain and installs whatever is missing
#      (compiler, CMake, git, Ninja, vcpkg build deps, SDL3 system -dev libs)
#   3. finds an existing vcpkg or clones + bootstraps one into ~/vcpkg
#   4. initialises the IconFontCppHeaders submodule
#   5. configures and builds the project through the vcpkg toolchain
#   6. stages the executable + public/ assets + .env + a launcher into ./dist
#
# NOTE ON PORTABILITY: the project's CMakeLists.txt forces `-march=native` for
# GCC/Clang, so the produced binary is tuned for THIS machine's CPU. Treat the
# dist/ folder as "runs on this box (and CPU-compatible siblings)", not as a
# universally portable build.
#
# Usage:
#   ./compile-nix.sh [BUILD_TYPE] [--no-install] [--clean] [--no-package] [-h]
#
#     BUILD_TYPE     Release (default) | Debug | Test
#     --no-install   only check prerequisites; never call sudo / a package
#                    manager. Abort with instructions on the first gap.
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
VCPKG_REPO="https://github.com/microsoft/vcpkg.git"

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

print_usage() {
    sed -n '2,40p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
}

for arg in "$@"; do
    case "$arg" in
        Release|Debug|Test)  BUILD_TYPE="$arg" ;;
        --no-install)        DO_INSTALL=0 ;;
        --clean)             DO_CLEAN=1 ;;
        --no-package)         DO_PACKAGE=0 ;;
        -h|--help)           print_usage; exit 0 ;;
        *)                   die "Unknown argument: '$arg' (try --help)" ;;
    esac
done

# ---------------------------------------------------------------------------
# --- Environment detection
# ---------------------------------------------------------------------------
step "Detecting environment"

[ "$(uname -s)" = "Linux" ] || die "This script is for Linux. Use compile-osx.sh on macOS."

ARCH="$(uname -m)"
NPROC=$( (command -v nproc >/dev/null && nproc) || echo 4 )

PKG=""            # apt | dnf
PKG_INSTALL=""    # full install command prefix
SUDO=""
[ "$(id -u)" -ne 0 ] && SUDO="sudo"

DISTRO_ID=""
DISTRO_NAME=""
if [ -r /etc/os-release ]; then
    # shellcheck disable=SC1091
    . /etc/os-release
    DISTRO_ID="${ID:-}"
    DISTRO_NAME="${PRETTY_NAME:-${NAME:-Linux}}"
fi

if command -v apt-get >/dev/null 2>&1; then
    PKG="apt"
    PKG_INSTALL="${SUDO} apt-get install -y"
elif command -v dnf >/dev/null 2>&1; then
    PKG="dnf"
    PKG_INSTALL="${SUDO} dnf install -y"
elif command -v yum >/dev/null 2>&1; then
    PKG="dnf"
    PKG_INSTALL="${SUDO} yum install -y"
fi

info "Distro     : ${DISTRO_NAME:-unknown} (${DISTRO_ID:-?})"
info "Arch       : ${ARCH}"
info "CPUs       : ${NPROC}"
info "Pkg manager: ${PKG:-none detected}"
info "Build type : ${BUILD_TYPE}"

APT_UPDATED=0

# Install a list of packages, prompting once before the privileged call.
# $1 = human label, rest = package names.
install_pkgs() {
    local label="$1"; shift
    local pkgs=("$@")

    if [ "$DO_INSTALL" -eq 0 ]; then
        die "$label missing and --no-install was given. Install manually:
    ${PKG_INSTALL} ${pkgs[*]}"
    fi
    [ -n "$PKG" ] || die "$label missing and no supported package manager (apt/dnf) was found.
    Install these manually: ${pkgs[*]}"

    warn "About to install ${label}: ${pkgs[*]}"
    warn "Command: ${PKG_INSTALL} ${pkgs[*]}"
    if [ -t 0 ]; then
        read -r -p "    Proceed? [Y/n] " reply
        case "$reply" in [nN]*) die "Aborted by user." ;; esac
    fi

    if [ "$PKG" = "apt" ] && [ "$APT_UPDATED" -eq 0 ]; then
        ${SUDO} apt-get update
        APT_UPDATED=1
    fi
    # shellcheck disable=SC2086
    ${PKG_INSTALL} "${pkgs[@]}"
}

# ---------------------------------------------------------------------------
# --- Toolchain: compiler + core tools
# ---------------------------------------------------------------------------
step "Checking build toolchain"

# --- C++ compiler ---------------------------------------------------------
if ! command -v g++ >/dev/null 2>&1 && ! command -v c++ >/dev/null 2>&1; then
    case "$PKG" in
        apt) install_pkgs "C++ compiler + build tools" build-essential ;;
        dnf) install_pkgs "C++ compiler + build tools" gcc-c++ make ;;
        *)   die "No C++ compiler found and no package manager to install one." ;;
    esac
fi
CXX_BIN="$(command -v g++ || command -v c++)"
CXX_VER="$("$CXX_BIN" -dumpfullversion -dumpversion 2>/dev/null | cut -d. -f1)"
if [ -n "$CXX_VER" ] && [ "$CXX_VER" -lt 13 ] 2>/dev/null; then
    warn "${CXX_BIN} is major version ${CXX_VER}; C++23 needs GCC >= 13 (or Clang >= 16)."
    warn "The build may fail. Install a newer compiler if it does."
else
    ok "C++ compiler: ${CXX_BIN} (major ${CXX_VER:-?})"
fi

# --- CMake (>= 3.21) ----------------------------------------------------
need_cmake=0
if command -v cmake >/dev/null 2>&1; then
    CM_VER="$(cmake --version | head -1 | grep -oE '[0-9]+\.[0-9]+' | head -1)"
    CM_MAJOR="${CM_VER%%.*}"; CM_MINOR="${CM_VER##*.}"
    if [ "$CM_MAJOR" -lt 3 ] || { [ "$CM_MAJOR" -eq 3 ] && [ "$CM_MINOR" -lt 21 ]; }; then
        warn "CMake ${CM_VER} is too old (need >= 3.21)."
        need_cmake=1
    else
        ok "CMake ${CM_VER}"
    fi
else
    need_cmake=1
fi
if [ "$need_cmake" -eq 1 ]; then
    install_pkgs "CMake" cmake
    CM_VER="$(cmake --version | head -1 | grep -oE '[0-9]+\.[0-9]+' | head -1)"
    CM_MAJOR="${CM_VER%%.*}"; CM_MINOR="${CM_VER##*.}"
    if [ "$CM_MAJOR" -lt 3 ] || { [ "$CM_MAJOR" -eq 3 ] && [ "$CM_MINOR" -lt 21 ]; }; then
        die "Distro CMake is still ${CM_VER}. Install a newer one from https://apt.kitware.com
    or https://cmake.org/download/ and re-run."
    fi
    ok "CMake ${CM_VER}"
fi

# --- git ----------------------------------------------------------------
if ! command -v git >/dev/null 2>&1; then
    install_pkgs "git" git
fi
ok "git $(git --version | awk '{print $3}')"

# --- Ninja (optional but preferred) -----------------------------------
GENERATOR_ARGS=()
if command -v ninja >/dev/null 2>&1; then
    ok "Ninja $(ninja --version)"
    GENERATOR_ARGS=(-G Ninja)
elif [ "$DO_INSTALL" -eq 1 ] && [ -n "$PKG" ]; then
    case "$PKG" in
        apt) install_pkgs "Ninja (optional)" ninja-build || true ;;
        dnf) install_pkgs "Ninja (optional)" ninja-build || true ;;
    esac
    if command -v ninja >/dev/null 2>&1; then
        ok "Ninja $(ninja --version)"
        GENERATOR_ARGS=(-G Ninja)
    else
        warn "Ninja not available; using the default Make generator."
    fi
else
    warn "Ninja not found; using the default Make generator."
fi

# ---------------------------------------------------------------------------
# --- vcpkg build dependencies (tools it needs to build ports from source)
# ---------------------------------------------------------------------------
step "Checking vcpkg build dependencies"

missing_builddeps=()
for tool in autoconf automake libtool pkg-config curl zip unzip tar; do
    command -v "$tool" >/dev/null 2>&1 || missing_builddeps+=("$tool")
done
# python3 + jinja2 are needed by some ports (e.g. wayland-protocols codegen)
if ! command -v python3 >/dev/null 2>&1; then
    missing_builddeps+=("python3")
fi

if [ "${#missing_builddeps[@]}" -gt 0 ]; then
    case "$PKG" in
        apt) install_pkgs "vcpkg build dependencies" \
                 autoconf autoconf-archive automake libtool pkg-config \
                 python3-jinja2 curl zip unzip tar ;;
        dnf) install_pkgs "vcpkg build dependencies" \
                 autoconf autoconf-archive automake libtool pkgconfig \
                 python3-jinja2 curl zip unzip tar ;;
        *)   die "Missing tools: ${missing_builddeps[*]} and no package manager to install them." ;;
    esac
else
    ok "core build tools present (autoconf, automake, libtool, pkg-config, curl, zip, tar)"
fi

# jinja2 module check (best effort — the -dev pkg above provides it on apt/dnf)
if command -v python3 >/dev/null 2>&1 && ! python3 -c 'import jinja2' >/dev/null 2>&1; then
    if [ "$DO_INSTALL" -eq 1 ]; then
        case "$PKG" in
            apt) install_pkgs "python3 jinja2" python3-jinja2 || true ;;
            dnf) install_pkgs "python3 jinja2" python3-jinja2 || true ;;
        esac
    fi
    python3 -c 'import jinja2' >/dev/null 2>&1 \
        && ok "python3 jinja2 module present" \
        || warn "python3 'jinja2' not importable; a vcpkg port may fail. pip install jinja2 if so."
else
    [ -x "$(command -v python3)" ] && ok "python3 + jinja2 present"
fi

# ---------------------------------------------------------------------------
# --- SDL3 system link libraries (X11 / Wayland / EGL / ibus headers)
# ---------------------------------------------------------------------------
step "Checking SDL3 system libraries"

# Probe with pkg-config where possible; fall back to header presence.
sdl_libs_ok=1
if command -v pkg-config >/dev/null 2>&1; then
    for pc in x11 xext xkbcommon egl; do
        pkg-config --exists "$pc" 2>/dev/null || sdl_libs_ok=0
    done
else
    sdl_libs_ok=0
fi

if [ "$sdl_libs_ok" -eq 1 ]; then
    ok "X11 / Xext / xkbcommon / EGL development files present"
else
    case "$PKG" in
        apt) install_pkgs "SDL3 system libraries" \
                 libx11-dev libxft-dev libxext-dev \
                 libwayland-dev libxkbcommon-dev \
                 libegl1-mesa-dev libibus-1.0-dev ;;
        dnf) install_pkgs "SDL3 system libraries" \
                 libX11-devel libXft-devel libXext-devel \
                 wayland-devel libxkbcommon-devel \
                 mesa-libEGL-devel ibus-devel ;;
        *)   warn "Could not verify SDL3 system libs and no package manager available.
    If the SDL3 build fails, install the X11/Wayland/EGL/ibus -dev packages." ;;
    esac
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
    for cand in "/opt/vcpkg" "${HOME}/vcpkg" "${HOME}/.vcpkg" "${HOME}/src/vcpkg"; do
        if vcpkg_valid "$cand"; then
            VCPKG_DIR="$cand"
            ok "Found vcpkg at ${VCPKG_DIR}"
            break
        fi
    done
fi

if [ -z "$VCPKG_DIR" ]; then
    TARGET="${HOME}/vcpkg"
    step "Cloning vcpkg into ${TARGET}"
    [ "$DO_INSTALL" -eq 1 ] || die "vcpkg not found and --no-install was given.
    git clone ${VCPKG_REPO} ${TARGET} && ${TARGET}/bootstrap-vcpkg.sh"
    if [ -d "$TARGET/.git" ]; then
        info "Directory exists; pulling latest"
        git -C "$TARGET" pull --ff-only || warn "git pull failed; continuing with existing checkout"
    else
        git clone "$VCPKG_REPO" "$TARGET"
    fi
    "$TARGET/bootstrap-vcpkg.sh"
    vcpkg_valid "$TARGET" || die "vcpkg bootstrap did not produce the toolchain file."
    VCPKG_DIR="$TARGET"
    ok "vcpkg ready at ${VCPKG_DIR}"
fi

export VCPKG_ROOT="$VCPKG_DIR"
if [ -z "${VCPKG_ROOT_PERSISTED:-}" ] && ! grep -qs "VCPKG_ROOT=" "${HOME}/.bashrc" "${HOME}/.zshrc" 2>/dev/null; then
    info "Tip: add 'export VCPKG_ROOT=\"${VCPKG_DIR}\"' to your ~/.bashrc or ~/.zshrc to persist it."
fi

# ---------------------------------------------------------------------------
# --- Submodules
# ---------------------------------------------------------------------------
step "Initialising submodules"
if [ -e "${REPO_ROOT}/.git" ]; then
    git -C "$REPO_ROOT" submodule update --init --recursive
    ok "submodules up to date"
else
    if [ -f "${REPO_ROOT}/lib/IconFontCppHeaders/IconsFontAwesome6.h" ]; then
        warn "Not a git checkout; assuming vendored lib/IconFontCppHeaders is present."
    else
        die "Not a git checkout and lib/IconFontCppHeaders is missing.
    Clone the repo with git (so submodules resolve) instead of downloading a zip."
    fi
fi

# ---------------------------------------------------------------------------
# --- Configure
# ---------------------------------------------------------------------------
step "Configuring (${BUILD_TYPE})"

if [ "$DO_CLEAN" -eq 1 ] && [ -d "$BUILD_DIR" ]; then
    info "Removing ${BUILD_DIR}"
    rm -rf "$BUILD_DIR"
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
for cand in \
    "${BUILD_DIR}/${BIN_NAME}" \
    "${BUILD_DIR}/${BUILD_TYPE}/${BIN_NAME}"; do
    if [ -x "$cand" ]; then BIN_PATH="$cand"; break; fi
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
# --- Stage into dist/
# ---------------------------------------------------------------------------
step "Staging runnable artifact into ${DIST_DIR}"

rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

install -m 0755 "$BIN_PATH" "${DIST_DIR}/${BIN_NAME}"
cp -R "${REPO_ROOT}/public" "${DIST_DIR}/public"

if [ -f "${REPO_ROOT}/.env" ]; then
    cp "${REPO_ROOT}/.env" "${DIST_DIR}/.env"
    ok ".env copied"
elif [ -f "${REPO_ROOT}/.env.example" ]; then
    cp "${REPO_ROOT}/.env.example" "${DIST_DIR}/.env"
    warn ".env not found — copied .env.example as .env (review it before shipping)"
else
    warn "no .env or .env.example found; the app may complain at startup"
fi

# Bundle any non-system shared libs the binary pulls from vcpkg_installed.
# With the default static x64-linux triplet this is normally empty.
mkdir -p "${DIST_DIR}/lib"
bundled=0
while read -r _name _arrow libpath _addr; do
    case "$libpath" in
        "${BUILD_DIR}/vcpkg_installed"/*)
            if [ -f "$libpath" ]; then
                cp -u "$libpath" "${DIST_DIR}/lib/"
                bundled=$((bundled + 1))
            fi
            ;;
    esac
done < <(ldd "$BIN_PATH" 2>/dev/null || true)

if [ "$bundled" -gt 0 ]; then
    ok "bundled ${bundled} shared lib(s) into dist/lib"
    if command -v patchelf >/dev/null 2>&1; then
        patchelf --set-rpath '$ORIGIN/lib' "${DIST_DIR}/${BIN_NAME}"
        ok "set RPATH \$ORIGIN/lib via patchelf"
    else
        warn "patchelf not installed — run.sh will set LD_LIBRARY_PATH instead"
    fi
else
    rmdir "${DIST_DIR}/lib" 2>/dev/null || true
fi

# Launcher — guarantees public/ and .env are in the working directory.
cat > "${DIST_DIR}/run.sh" <<EOF
#!/usr/bin/env bash
# Launcher for Adventure Designer Studio — keeps assets on the working dir.
here="\$(cd "\$(dirname "\${BASH_SOURCE[0]}")" && pwd)"
cd "\$here"
[ -d "\$here/lib" ] && export LD_LIBRARY_PATH="\$here/lib:\${LD_LIBRARY_PATH:-}"
exec "\$here/${BIN_NAME}" "\$@"
EOF
chmod +x "${DIST_DIR}/run.sh"

# ---------------------------------------------------------------------------
# --- Sanity check
# ---------------------------------------------------------------------------
step "Verifying artifact"
missing_syms="$(ldd "${DIST_DIR}/${BIN_NAME}" 2>/dev/null | grep -i 'not found' || true)"
if [ -n "$missing_syms" ]; then
    warn "Unresolved shared libraries:"
    printf '%s\n' "$missing_syms" | sed 's/^/        /'
    warn "The target machine will need these system libraries installed."
else
    ok "all shared libraries resolve"
fi
[ -f "${DIST_DIR}/public/translations/core/en_US.json" ] \
    && ok "assets staged (public/translations/core/en_US.json present)" \
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

    Run it:
      ${DIST_DIR}/run.sh

    Ship it:
      tar -czf ADS-linux-${ARCH}.tar.gz -C '${REPO_ROOT}' dist

    Reminder: the binary is built with -march=native — it targets this machine's
    CPU. Rebuild on the target if it differs significantly.
EOF
