<p align="center"><img src="public/assets/logo.png" width="564" alt="Adventure Studio Designer Logo"></p>

# Adventure Studio Designer

**Adventure Studio Designer** was born with the idea of giving life back to a type of game that was embraced by an entire generation, conversational adventures. Surely you remember adventures like the [Hobbit](https://bbcmicro.co.uk/jsbeeb/play.php?autoboot&disc=https://bbcmicro.co.uk/gameimg/discs/1681/Disc094-HobbitSTT.ssd&noseek), [Zork](https://es.wikipedia.org/wiki/Zork) or, in Spain, any of the text adventures created by the company [Aventuras AD](https://es.wikipedia.org/wiki/Aventuras_AD) like [La Aventura Original](https://es.wikipedia.org/wiki/La_Aventura_Original), [Jabato](https://es.wikipedia.org/w/index.php?title=Jabato_(videojuego)&action=edit&redlink=1) o [La Diosa de Cozumel](https://es.wikipedia.org/w/index.php?title=La_diosa_de_Cozumel&action=edit&redlink=1).

Nowadays, a small bunch of people around the world are still creating, playing and distributing this kind of game. In Spain,  the most significant public organisation is [CAAD](https://caad.club/), which provides news, support, forums and a distribution channel for those games.

I was one of those people who spent countless hours playing Text Adventures —also known in Spanish as "Aventuras Conversacionales". I believe that with today's technologies, we can breathe new life into a new genre called **Enhanced Conversational Adventures**. These modern versions could incorporate voice recognition and text-to-speech for accessibility, include video and animated scenes to enhance immersion, and even use Augmented Reality to bring adventures into the real world.

I strongly believe it's possible to empower anyone to create their own Text Adventure without needing any programming knowledge. By providing an intuitive Editor —where users only need to point, create, connect, and compile— we can make game creation accessible to everyone.

Imagine a teacher using such a tool to build a short adventure that re-creates the French Revolution. With no coding skills required, students could explore the events as participants, gaining a deeper understanding of history by experiencing it interactively— even exploring alternate outcomes, like preventing the revolution entirely.

## Getting Started

### Prerequisites

- **C++ Compiler**: GCC 11+ or Clang 14+ (C++23 support required)
- **CMake**: Version 3.21 or higher
- **Git**: For cloning the repository and vcpkg
- **vcpkg**: Package manager for C++ libraries
- **System Build Tools**: Required by vcpkg to build certain dependencies

### How to Install Prerequisites

<details>
<summary><strong>Linux (Debian/Ubuntu)</strong></summary>

**1. Install C++ Compiler and Build Tools:**
```bash
sudo apt update
sudo apt install build-essential autoconf automake libtool pkg-config python3-jinja2
```
This installs GCC, G++, Make, and other essential build tools.

**2. Install CMake:**
```bash
sudo apt install cmake
```
If you need a newer version (3.21+), use the official Kitware repository:
```bash
sudo apt install software-properties-common
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update
sudo apt install cmake
```

**3. Install Git:**
```bash
sudo apt install git
```

**4. Verify installations:**
```bash
g++ --version      # Should show GCC 11+
cmake --version    # Should show 3.21+
git --version
```

</details>

<details>
<summary><strong>Linux (Fedora/RHEL)</strong></summary>

**1. Install C++ Compiler and Build Tools:**
```bash
sudo dnf install gcc-c++ autoconf automake libtool pkgconfig python3-jinja2
```

**2. Install CMake:**
```bash
sudo dnf install cmake
```

**3. Install Git:**
```bash
sudo dnf install git
```

**4. Verify installations:**
```bash
g++ --version      # Should show GCC 11+
cmake --version    # Should show 3.21+
git --version
```

</details>

<details>
<summary><strong>macOS</strong></summary>

**1. Install Xcode Command Line Tools (includes Clang compiler):**
```bash
xcode-select --install
```

**2. Install Homebrew (if not already installed):**
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

**3. Install CMake and Build Tools:**
```bash
brew install cmake autoconf automake libtool pkg-config
pip3 install jinja2
```

**4. Install Git (if not included with Xcode tools):**
```bash
brew install git
```

**5. Verify installations:**
```bash
clang++ --version  # Should show Clang 14+
cmake --version    # Should show 3.21+
git --version
```

</details>

<details>
<summary><strong>Windows</strong></summary>

**1. Install Visual Studio Build Tools (C++ Compiler):**
- Download [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
- Run the installer and select **"Desktop development with C++"**
- This includes MSVC compiler and Windows SDK

**2. Install CMake:**
- Download the installer from [cmake.org/download](https://cmake.org/download/)
- Choose the **Windows x64 Installer (.msi)**
- **Important:** During installation, select **"Add CMake to the system PATH for all users"**

**3. Install Git:**
- Download from [git-scm.com](https://git-scm.com/download/win)
- Run the installer with default options
- **Recommended:** Select "Git from the command line and also from 3rd-party software"

**4. Restart your terminal** (Command Prompt or PowerShell) after all installations

**5. Verify installations:**

From a regular Command Prompt or PowerShell:
```batch
cmake --version    REM Should show 3.21+
git --version
```

To verify the C++ compiler, use one of these methods:

*Option A:* Open **"Developer Command Prompt for VS"** or **"Developer PowerShell for VS"** from the Start Menu, then run:
```batch
cl
```
This should display Microsoft C/C++ compiler version info.

*Option B:* From a regular Command Prompt, run:
```batch
where /R "C:\Program Files\Microsoft Visual Studio" cl.exe
```
This should find the cl.exe compiler path if Visual Studio Build Tools is installed correctly.

*Option C:* Open **Visual Studio Installer** and verify that **"Desktop development with C++"** workload is installed.

</details>

### Quick Start

#### 1. Install vcpkg

vcpkg manages all project dependencies automatically. Install it system-wide:

**Linux/macOS:**
```bash
# Clone vcpkg to /opt
sudo git clone https://github.com/microsoft/vcpkg.git /opt/vcpkg

# Change ownership to your user
sudo chown -R $USER:$USER /opt/vcpkg

# Bootstrap vcpkg
cd /opt/vcpkg
./bootstrap-vcpkg.sh

# Set environment variable (add to ~/.bashrc or ~/.zshrc for persistence)
export VCPKG_ROOT=/opt/vcpkg
echo 'export VCPKG_ROOT=/opt/vcpkg' >> ~/.bashrc
```

**Windows:**
```powershell
# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg

# Bootstrap vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat 

# Set environment variable
setx VCPKG_ROOT "C:\vcpkg"
```

#### 2. Clone the Repository

```bash
git clone https://github.com/chernandezosma/Adventure-Designer-Studio.git
cd Adventure-Designer-Studio
git submodule update --init --recursive
```

#### 3. (Optional) Setup Reference Libraries

For code browsing in your IDE (not required for building):

```bash
./setup-reference-libs.sh
```

This clones imgui, nlohmann_json, and spdlog to `lib/` for reference only. The build uses vcpkg versions.

#### 4. Build the Project

**Linux/macOS:**
```bash
./build.sh           # Release build (default)
./build.sh Debug     # Debug build
./build.sh Test      # Test build with unit tests
```

**Windows:**
```batch
build.bat            REM Release build (default)
build.bat Debug      REM Debug build
build.bat Test       REM Test build
```

**Note:** The first build takes 10-20 minutes as vcpkg downloads and compiles all dependencies. Subsequent builds are much faster.

#### 5. Run the Application

**Linux/macOS:**
```bash
cd build
./Adventure_Designer_Studio
```

**Windows:**
```batch
cd build\Release
Adventure_Designer_Studio.exe
```

### Project Dependencies

Managed by vcpkg:
- **imgui** - Immediate mode GUI library (with SDL2 bindings and docking)
- **SDL2** - Graphics, windowing, and input
- **nlohmann-json** - JSON library
- **spdlog** - Fast C++ logging library
- **boost-uuid** - UUID generation
- **gtest** - Google Test framework

Git submodule:
- **IconFontCppHeaders** - Header-only icon font integration

### Build Troubleshooting

**"Cannot find vcpkg"**

*Linux/macOS:*
```bash
echo $VCPKG_ROOT  # Should print /opt/vcpkg
```
If empty, set it: `export VCPKG_ROOT=/opt/vcpkg`

*Windows:*
```batch
echo %VCPKG_ROOT%  REM Should print C:\vcpkg
```
If empty, set it: `setx VCPKG_ROOT "C:\vcpkg"` and restart your terminal.

**"cmake is not recognized" (Windows)**

CMake is not installed or not in PATH. Install CMake from [cmake.org/download](https://cmake.org/download/) and ensure you select **"Add CMake to the system PATH"** during installation. Restart your terminal after installation.

**CMake configuration fails**

*Linux/macOS:*
```bash
# Clean build and try again
rm -rf build
./build.sh
```

*Windows:*
```batch
REM Clean build and try again
rmdir /s /q build
build.bat
```

**Windows: Do I need Developer Command Prompt?**

No. You can run `build.bat` from a regular Command Prompt or PowerShell. CMake automatically detects Visual Studio Build Tools. Just ensure:
1. CMake is in your PATH
2. Visual Studio Build Tools with "Desktop development with C++" is installed

**For detailed migration information**, see [VCPKG_MIGRATION.md](./VCPKG_MIGRATION.md) and [LIBRARY_SETUP.md](./LIBRARY_SETUP.md).