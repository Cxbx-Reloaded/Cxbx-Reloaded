# Cxbx-Reloaded - Original Xbox Emulator 
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://img.shields.io/badge/License-GPL%20v2-blue.svg)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/iao43irxl3umbp33?svg=true)](https://ci.appveyor.com/project/SoullessSentinel/cxbx-reloaded)
[![Discord](https://img.shields.io/badge/chat-on%20discord-7289da.svg?logo=discord)](https://discord.gg/26Xjx23)

Cxbx-Reloaded is an emulator for running Microsoft Xbox (and eventually, Chihiro) games on Microsoft Windows.

## System Requirements
### Minimum
  * OS: Windows 7+ x64. 32-bit installations are not supported.
  * GPU: Anything that supports Direct3D 9 (HLE) and OpenGL 3.3 (LLE).

### Prerequisites
  * 32-bit (x86) Visual C++ 2019 redistributable is required. Download it [here](https://support.microsoft.com/en-gb/help/2977003/the-latest-supported-visual-c-downloads).
    * Make sure to install the 32-bit (x86) version even if you're on a 64-bit machine, as the 64-bit build does not include both!
  * Npcap (Used for Network emulation): Download the Installer [here](https://nmap.org/npcap/#download). Make sure to enable winpcap compatibility mode!

## Automated Builds
Cxbx-Reloaded is not yet ready for general release, but the latest development builds can be downloaded using the links below:

### Development Builds
* [Latest Development Build](https://ci.appveyor.com/api/projects/SoullessSentinel/cxbx-reloaded/artifacts/Release.zip?branch=develop&job=Configuration:%20Release&pr=false)

* [Full build history](https://ci.appveyor.com/project/SoullessSentinel/cxbx-reloaded/history)

## Compatibility
Cxbx-Reloaded has a [compatibility list](https://github.com/Cxbx-Reloaded/game-compatibility/issues).

If you have something to report on a title, please create or update the issue for it there.

Please read the [Readme file](https://github.com/Cxbx-Reloaded/game-compatibility/blob/master/README.md) first!

## Bug Reports
Game or software specific issues can be reported in the [compatibility list](https://github.com/Cxbx-Reloaded/game-compatibility/issues).

For emulation issues that are not specific to any single piece of software, a bug report can be submitted at [the Cxbx-Reloaded issue tracker](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues).

Make sure bug reports contain:
* The build tested with, error message displayed (if any)
* Screenshots 
* Xbe dump (created via [Edit > Dump Xbe Info To > File])
* Kernel Debug log (created when running a game with [View > Debug Output (Kernel) > File] selected).

## Additional information
Cxbx-Reloaded has a [wiki](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki) containing various subjects and background information.
Chat on [Discord](https://discord.gg/26Xjx23).

## Contributing
We welcome contributions, large and small.

If you want to do some coding, be sure to read the [Developer notes](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki/Developer-notes).

**IMPORTANT: Pull-Requests containing code derived from XQEMU will _not_ be approved until an agreement is reached to make work mutually beneficial. this includes updates to existing xqemu derived code. We should not/will not become a hostile fork.**

Please contact us before you start working on something, so we can make sure your work is going to be accepted once finished.

### Main Prerequisites
1. [Git for Windows](https://git-scm.com/)
2. [CMake](https://cmake.org/)
   * **REMARK:** Some IDEs already have CMake support, so this is optional.

### Fetching the code
Run the following command on the command line:
`git clone --recurse-submodules https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/`

Please note the `--recurse-submodules` parameter. This is required to fetch sub-modules.

Without this, Cxbx-Reloaded will fail to build.

If Cxbx-Reloaded was previously checked out without the submodules, they can be updated/fetched with the following command:
`git submodule update --init --recursive`

### Compiling
Please note that Debug builds are significantly slower, but may help diagnose a problematic title.

## Windows
**NOTE:** It's not recommended to open CMakeLists.txt from Visual Studio, as it won't generate files in the build directory.

### Prerequisites
1. [Visual Studio 2019](https://www.visualstudio.com/downloads/) or 2017
   * Both C++ and C# desktop development
   * Windows Universal CRT SDK
   * C++ CMake tools for Windows **(optional if CMake is installed on system)**
2. [Microsoft Child Process Debugging Power Tool](https://marketplace.visualstudio.com/items?itemName=GreggMiskelly.MicrosoftChildProcessDebuggingPowerTool)

### Create Visual Studio generator files
1. Open "___ Native Tools Command Prompt for VS 20__" from Start Menu.
   * Ignore this step if you have CMake installed on your system.
2. Navigate to downloaded Cxbx-Reloaded repo's directory as current working directory.
3. Then run these commands.
   1. `mkdir build & cd build` - Requirement to create and go in the build directory.
      * **REMARK:** You can use `build-x86` as your preferred directory as well.
   2. `cmake .. -G "Visual Studio 16 2019" -A Win32` - Create generated files for Visual Studio 2019.
      * Or use `"Visual Studio 15 2017"` for Visual Studio 2017.
        * **REMARK:** CMake 1.14 or later is the requirement to support Visual Studio 2019 generator.
          * Visual Studio **16.1** 2019 or later has CMake 1.14 bundled.
4. Open `Cxbx-Reloaded.sln` from build-x86 directory, select your configuration (Release or Debug), then hit Build.

## Linux / macOS
Currently not supported yet.

## Support
You can support [Luke Usher](https://github.com/LukeUsher), initiator of Cxbx-Reloaded, on [Patreon](https://www.patreon.com/LukeUsher).

## Special Thanks
* All contributors to the original Cxbx and [Dxbx](https://github.com/PatrickvL/Dxbx) projects. Without them Cxbx-Reloaded would not exist at all.
* [XQEMU](https://github.com/xqemu/xqemu) - While the majority of Cxbx-R is our own work (Kernel, HLE, etc), the NV2A LLE implementation and NVNet implementation are primarily the work of the XQEMU developers.
* [XboxDev](https://github.com/xboxdev/) - Providing Xbox Hardware Research & Useful Tooling
