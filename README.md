# Cxbx-Reloaded - Original Xbox Emulator
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/blob/master/COPYING)
[![GitHub Actions](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/workflows/GitHub%20CI/badge.svg?event=push)](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/actions?query=event%3Apush+workflow%3A%22GitHub+CI%22)
[![Discord](https://img.shields.io/badge/chat-on%20discord-7289da.svg?logo=discord)](https://discord.gg/26Xjx23)

Cxbx-Reloaded is an emulator for running Microsoft Xbox (and eventually, Chihiro) games on Microsoft Windows and Wine.

## System Requirements
### Minimum
  * OS: Windows 7+ x64, or x86-64 Linux with Wine. 32-bit is not supported.
    * MacOS with Wine is known not to work, and BSD-based systems are untested.
    * Also note that Wine is relatively unstable, and it might break compatibility with Cxbx-Reloaded at any time without warning.
  * GPU: Direct3D 9.0c with Pixel Shader Model 2.x, and Vertex Shader Model 3.0.

## Prerequisites
### Windows
  * [32-bit (x86) Visual C++ 2022 Redistributable](https://aka.ms/vs/17/release/vc_redist.x86.exe)
  * [Npcap *(used for network emulation)*](https://nmap.org/npcap/#download)
    * Make sure to enable winpcap compatibility mode.
  * [WinUSB compliant driver](https://github.com/libusb/libusb/wiki/Windows#Driver_Installation)
    * *Optional, only needed for USB pass-through of original Xbox and Steel Battalion controllers.*

### Wine
**Please use the latest stable release version of Wine. If it does not work for you, then roll back to Wine 7.0 which is the last known working version.**<br/>
**There also exists this known [issue](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/2314) which currently prevents savings in some games with the most recent Wine 6.8 and later versions.**
  * Winetricks
    * `vcrun2019`
      * Requires the latest winetricks script.
    * `d3dcompiler_47`
      * This may be subject to change.
  * Winpcap is built-in, no installation is required.

## Automated Builds
Cxbx-Reloaded doesn't currently have stable builds, but you can obtain pre-release builds from our official website's download page, or the links below:

  * **[Release Builds](https://cxbx-reloaded.co.uk/download)**
  * *[Full build history](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/actions?query=workflow%3A%22GitHub+CI%22)*

## Compatibility
Cxbx-Reloaded has a [compatibility list](https://cxbx-reloaded.co.uk/compatibility).

If you would like to submit compatibility reports, please request permission in our Discord server.

## Bug Reports
Game or software specific issues can be reported in the [compatibility website](https://cxbx-reloaded.co.uk/compatibility).

For emulation issues that are not specific to any single piece of software, a bug report can be submitted at [the Cxbx-Reloaded issue tracker](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues).
<!--Make sure to follow the issue template and that it contains:
  * The build tested with, error message displayed (if any)
    * **You can copy and paste any popup messages. However, please keep it clean by pasting and trimming down to only the message itself.**
  * Screenshots
    * Optional unless there are graphic bugs for reference.

**Failure to follow the template will auto close your ticket.**-->

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
<br>`git clone --recurse-submodules https://github.com/Cxbx-Reloaded/Cxbx-Reloaded.git`
    * Please note the `--recurse-submodules` parameter. This is required to fetch submodules.
      * If Cxbx-Reloaded was checked out without submodules, they can be updated/fetched with the following command:

        `git submodule update --init --recursive`

### Compiling

#### Windows
Don't open `CMakeLists.txt` from Visual Studio, as it won't generate files in the `build` directory.

##### Prerequisites
1. [Visual Studio](https://visualstudio.microsoft.com/downloads/) 2022
    * C++ and C# desktop development
    * Windows Universal CRT SDK
    * C++ CMake tools for Windows
      * *Optional if CMake is installed*
    * [Microsoft Child Process Debugging Power Tool](https://marketplace.visualstudio.com/items?itemName=vsdbgplat.MicrosoftChildProcessDebuggingPowerTool)

##### Generate Visual Studio files
1. If you don't have CMake installed, open `___ Native Tools Command Prompt for VS 20##`.
2. `cd` to the Cxbx-Reloaded directory.
3. Run the following command: `cmake -B build -G "Visual Studio 17 2022" -A Win32` \
   **NOTES**:
     * VS2022 17.0 or later is required.
     * To build the Cxbx-Reloaded Debugger tool, add the variable `-DBUILD_CXBXR_DEBUGGER=ON` to the above command.
       * _This debugger tool is deprecated and will be eventually removed, please use the Visual Studio debugger instead._
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
* [XbSymbolDatabase](https://github.com/Cxbx-Reloaded/XbSymbolDatabase) - Providing support to detect symbols across XDK builds from reverse engineered retail titles.
* [Xbox Kernel Test Suite](https://github.com/Cxbx-Reloaded/xbox_kernel_test_suite) - Making accurate tests on hardware to compare against cxbxr's kernel implementation.
