#!/bin/bash
set -e

# Adventure Designer Studio - Build Script for Linux/Mac
# This script builds the project using vcpkg for dependency management

# Configuration
BUILD_TYPE=${1:-Release}
VCPKG_ROOT=${VCPKG_ROOT:-/opt/vcpkg}

echo "========================================="
echo "Building Adventure Designer Studio"
echo "Build Type: $BUILD_TYPE"
echo "vcpkg Root: $VCPKG_ROOT"
echo "========================================="

# Check if vcpkg exists
if [ ! -d "$VCPKG_ROOT" ]; then
    echo "ERROR: vcpkg not found at $VCPKG_ROOT"
    echo "Please install vcpkg first:"
    echo "  sudo git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg"
    echo "  sudo chown -R \$USER:\$USER /opt/vcpkg"
    echo "  cd /opt/vcpkg"
    echo "  ./bootstrap-vcpkg.sh"
    echo ""
    echo "Or set VCPKG_ROOT environment variable to your vcpkg installation:"
    echo "  export VCPKG_ROOT=/path/to/vcpkg"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

echo ""
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DVCPKG_INSTALLED_DIR="$(pwd)/vcpkg_installed"

echo ""
echo "Building project..."
# Use nproc on Linux, sysctl on Mac
if command -v nproc > /dev/null; then
    NPROC=$(nproc)
elif command -v sysctl > /dev/null; then
    NPROC=$(sysctl -n hw.ncpu)
else
    NPROC=4
fi

cmake --build . --config $BUILD_TYPE -j$NPROC

echo ""
echo "========================================="
echo "Build complete!"
echo "========================================="
echo "Executable: build/Adventure_Designer_Studio"
echo ""
echo "To run the application:"
echo "  cd build && ./Adventure_Designer_Studio"
echo ""
