# Cxbx-Reloaded - Original Xbox Emulator
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/blob/master/COPYING)
[![GitHub Actions](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/workflows/GitHub%20CI/badge.svg?event=push)](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/actions?query=event%3Apush+workflow%3A%22GitHub+CI%22)
[![Discord](https://img.shields.io/badge/chat-on%20discord-7289da.svg?logo=discord)](https://discord.gg/26Xjx23)

Cxbx-Reloaded is an emulator for running Microsoft Xbox (and eventually, Chihiro) games on Microsoft Windows and Wine.

## System Requirements
### Minimum
  * OS: Windows 7+ x64, or x86-64 Linux with Wine. 32-bit is not supported.
  * GPU: Direct3D 9.0c with Pixel Shader Model 2.x, and Vertex Shader Model 3.0.

## Prerequisites
### Windows
  * [32-bit (x86) Visual C++ 2019 Redistributable](https://aka.ms/vs/16/release/vc_redist.x86.exe)
  * [Npcap *(used for network emulation)*](https://nmap.org/npcap/#download)
    * Make sure to enable winpcap compatibility mode!

### Wine
**NOTICE: Please use the latest stable release version of Wine. If it does not work for you, then roll back to Wine 5.0.3 which is the last known working version.**
  * Winetricks
    * `vcrun2017` or `vcrun2019`
      * NOTE: vcrun2019 requires the latest winetricks script!
    * `d3dcompiler_47`
      * NOTE: May be subject to change over time.
  * Winpcap is built-in, no installation is required.


## Automated Builds
Cxbx-Reloaded doesn't currently have stable builds, but you can obtain pre-release builds from the Releases tab, or the links below:

  * **[Release Builds](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/releases)**
    * **Wine users will need to use `CxbxReloaded-Release-VS2017.zip` for it to run correctly.**
  * *[Full build history](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/actions?query=workflow%3A%22GitHub+CI%22)*

## Compatibility
Cxbx-Reloaded has a [compatibility list](https://cxbx-reloaded.co.uk/compatibility).

If you would like to submit compatibility reports, please request permission in our Discord server.

## Bug Reports
Game or software specific issues can be reported in the [compatibility website](https://cxbx-reloaded.co.uk/compatibility).

For emulation issues that are not specific to any single piece of software, a bug report can be submitted at [the Cxbx-Reloaded issue tracker](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues).

Make sure to follow the issue template and that it contains:
  * The build tested with, error message displayed (if any)
    * **(You can copy and paste any popup messages. However, please keep it clean by paste and trimming down to only the message itself.)**
  * Screenshots
    * (optional unless has any graphic bug for references)

**NOTICE: Failure to follow template will auto close your ticket.**


## Additional information
Cxbx-Reloaded has a [wiki](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki) containing various subjects and background information.

Chat on [Discord](https://discord.gg/26Xjx23).

## Contributing
We welcome contributions, large and small.

If you want to do some coding, be sure to read the [Developer notes](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki/Developer-notes).

**IMPORTANT: Pull-Requests containing code derived from XQEMU will _not_ be approved until an agreement is reached to make work mutually beneficial. This includes updates to existing XQEMU derived code. We should not/will not become a hostile fork.**

Please contact us before you start working on something, so we can make sure your work is going to be accepted once finished.

### Main Prerequisites
1. [Git for Windows](https://git-scm.com)
2. [CMake](https://cmake.org)
    * Some IDEs already have CMake support, this is optional.

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
