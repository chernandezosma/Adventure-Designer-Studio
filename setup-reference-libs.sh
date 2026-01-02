#!/bin/bash
# Setup script for reference libraries (not tracked in git, for code browsing only)

set -e

echo "========================================="
echo "Setting up reference libraries..."
echo "========================================="
echo ""
echo "These libraries are for CODE REFERENCE ONLY."
echo "The actual build uses vcpkg-managed versions."
echo ""

# Create lib directory if it doesn't exist
mkdir -p lib
cd lib

# Clone imgui for reference (docking branch)
if [ ! -d "imgui" ]; then
    echo "Cloning imgui (docking branch)..."
    git clone --branch docking https://github.com/ocornut/imgui.git
    echo "✓ imgui cloned"
else
    echo "✓ imgui already exists"
fi

# Clone nlohmann_json for reference
if [ ! -d "nlohmann_json" ]; then
    echo "Cloning nlohmann_json..."
    git clone --branch develop https://github.com/nlohmann/json.git nlohmann_json
    echo "✓ nlohmann_json cloned"
else
    echo "✓ nlohmann_json already exists"
fi

# Clone spdlog for reference
if [ ! -d "spdlog" ]; then
    echo "Cloning spdlog..."
    git clone https://github.com/gabime/spdlog.git
    echo "✓ spdlog cloned"
else
    echo "✓ spdlog already exists"
fi

echo ""
echo "========================================="
echo "Reference libraries setup complete!"
echo "========================================="
echo ""
echo "Remember: These are for browsing only."
echo "The build uses vcpkg versions."
echo ""