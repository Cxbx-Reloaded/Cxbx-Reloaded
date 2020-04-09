# Cxbx-Reloaded - Original Xbox Emulator
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://img.shields.io/badge/License-GPL%20v2-blue.svg)
[![GitHub Actions](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/workflows/GitHub%20CI/badge.svg)](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/actions?query=workflow%3A%22GitHub+CI%22)
[![Azure](https://Cxbx-Reloaded.visualstudio.com/Cxbx-Reloaded/_apis/build/status/Cxbx-Reloaded.Cxbx-Reloaded?branchName=develop)](https://Cxbx-Reloaded.visualstudio.com/Cxbx-Reloaded/_build/latest?definitionId=7&branchName=develop)
[![Discord](https://img.shields.io/badge/chat-on%20discord-7289da.svg?logo=discord)](https://discord.gg/26Xjx23)

Cxbx-Reloaded is an emulator for running Microsoft Xbox (and eventually, Chihiro) games on Microsoft Windows.

## System Requirements
### Minimum
  * OS: Windows 7+ x64. 32-bit is not supported.
  * GPU: Direct3D 9.0c with Pixel Shader Model 2.x, and Vertex Shader Model 3.0.

### Prerequisites
  * [32-bit (x86) Visual C++ 2019 Redistributable](https://aka.ms/vs/16/release/vc_redist.x86.exe)
  * [Npcap *(used for network emulation)*](https://nmap.org/npcap/#download)
    * Make sure to enable winpcap compatibility mode!

## Automated Builds
Cxbx-Reloaded doesn't currently have stable builds, but you can obtain pre-release builds from the Releases tab, or the links below:

  * **[Release Builds](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/releases)**
    * **WINE users will need to use `CxbxReloaded-Release-VS2017.zip` for it to run correctly.**
  * *[Full build history](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/actions?query=workflow%3A%22GitHub+CI%22)*

## Compatibility
Cxbx-Reloaded has a [compatibility list](https://github.com/Cxbx-Reloaded/game-compatibility#cxbx-reloaded-game-compatibility-project).

If you have something to report on a title, please create or update the issue for it there.

Please read the [Readme file](https://github.com/Cxbx-Reloaded/game-compatibility/blob/master/README.md) first!

## Bug Reports
Game or software specific issues can be reported in the [compatibility list](https://github.com/Cxbx-Reloaded/game-compatibility#cxbx-reloaded-game-compatibility-project).

For emulation issues that are not specific to any single piece of software, a bug report can be submitted at [the Cxbx-Reloaded issue tracker](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues).

Make sure bug reports contain:
  * The build tested with, error message displayed (if any)
  * Screenshots 
  * Xbe dump (created via `Edit > Dump Xbe Info To > File`)
  * Kernel Debug log (created when running a game with `View > Debug Output (Kernel) > File` selected).

## Additional information
Cxbx-Reloaded has a [wiki](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki) containing various subjects and background information.

Chat on [Discord](https://discord.gg/26Xjx23).

## Contributing
We welcome contributions, large and small.

If you want to do some coding, be sure to read the [Developer notes](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki/Developer-notes).

**IMPORTANT: Pull-Requests containing code derived from XQEMU will _not_ be approved until an agreement is reached to make work mutually beneficial. this includes updates to existing XQEMU derived code. We should not/will not become a hostile fork.**

Please contact us before you start working on something, so we can make sure your work is going to be accepted once finished.

### Main Prerequisites
1. [Git for Windows](https://git-scm.com)
2. [CMake](https://cmake.org)
    * Some IDEs already have CMake support, so this is optional.

### Fetching the code
1. Run the following command in the command line:

    `git clone --recurse-submodules https://github.com/Cxbx-Reloaded/Cxbx-Reloaded.git`
    * Please note the `--recurse-submodules` parameter. This is required to fetch submodules.
      * If Cxbx-Reloaded was checked out without submodules, they can be updated/fetched with the following command:

        `git submodule update --init --recursive`

### Compiling

#### Windows
**NOTE:** Don't open `CMakeLists.txt` from Visual Studio, as it won't generate files in the `build` directory.

##### Prerequisites
1. [Visual Studio](https://visualstudio.microsoft.com/downloads/) 2017 or later
    * C++ and C# desktop development
    * Windows Universal CRT SDK
    * C++ CMake tools for Windows
      * *Optional if CMake is installed*
    * [Microsoft Child Process Debugging Power Tool](https://marketplace.visualstudio.com/items?itemName=vsdbgplat.MicrosoftChildProcessDebuggingPowerTool)

##### Generate Visual Studio files
1. If you don't have CMake installed, open `___ Native Tools Command Prompt for VS 20##`.
2. `cd` to the Cxbx-Reloaded directory.
3. Run these commands.
    1. `mkdir build & cd build`
    2. `cmake .. -G "Visual Studio 16 2019" -A Win32`
        * Visual Studio 2019 16.1 or later has CMake 3.14 bundled, and is required for the Visual Studio 2019 generator.
        * Use `cmake .. -G "Visual Studio 15 2017" -A Win32` for Visual Studio 2017.
4. Open `Cxbx-Reloaded.sln` from the `build` directory.
5. Select the Release configuration, then click Build.
    * Debug builds are **significantly slower, and only for developers**.

#### Linux / macOS
Currently not supported.

## Support
You can support [Luke Usher](https://github.com/LukeUsher), initiator of Cxbx-Reloaded, on [Patreon](https://www.patreon.com/LukeUsher).

## Special Thanks
* All contributors to the original Cxbx and [Dxbx](https://github.com/PatrickvL/Dxbx) projects. Without them Cxbx-Reloaded would not exist at all.
* [XQEMU](https://github.com/xqemu/xqemu) - While the majority of Cxbx-R is our own work (Kernel, HLE, etc), the NV2A LLE and NVNet implementation are primarily the work of the XQEMU developers.
* [XboxDev](https://github.com/xboxdev) - Providing Xbox hardware research & useful tooling.
