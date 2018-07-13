# Cxbx-Reloaded - Original Xbox Emulator 
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://img.shields.io/badge/License-GPL%20v2-blue.svg)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/iao43irxl3umbp33?svg=true)](https://ci.appveyor.com/project/SoullessSentinel/cxbx-reloaded)
[![Gitter](https://badges.gitter.im/gitterHQ/gitter.svg)](https://gitter.im/Cxbx-Reloaded/Lobby)
or [Discord](https://discord.gg/26Xjx23)

Cxbx-Reloaded is an emulator for running Microsoft Xbox (and eventually, Chihiro) games on Microsoft Windows.

## System Requirements
### Minimum
  * OS: Windows 7+ x64. 32-bit installations are not supported.
  * GPU: Anything that supports Direct3D 8 (HLE) and OpenGL 3.3 (LLE).
  * Due to poor driver support for Direct3D8 on modern cards, some users may see better results when using a wrapper, such as [D3D8to9](https://github.com/crosire/d3d8to9/)
### Prerequisites
  * Visual C++ 2015 and 2017 redistributables may be required. Download them [here](https://support.microsoft.com/en-gb/help/2977003/the-latest-supported-visual-c-downloads).
  
## Automated Builds
Cxbx-Reloaded is not yet ready for general release, but the latest development builds can be downloaded using the links below:

### Latest Stable
Grab these if you just want to use the emulator to play games.
* [Latest Stable (Release Build)](https://ci.appveyor.com/api/projects/SoullessSentinel/cxbx-reloaded/artifacts/export/Release.zip?branch=master&job=Configuration:%20Release&pr=false)
* [Latest Stable (Debug Build)](https://ci.appveyor.com/api/projects/SoullessSentinel/cxbx-reloaded/artifacts/export/Debug.zip?branch=master&job=Configuration:%20Debug&pr=false)

### Development Builds
Grab these builds if you want to test the latest and greatest features. These builds are likely to be more unstable than release builds, and are not recommended for just playing games.
* [Latest Development Build (Release)](https://ci.appveyor.com/api/projects/SoullessSentinel/cxbx-reloaded/artifacts/export/Release.zip?branch=develop&job=Configuration:%20Release&pr=false)
* [Latest Development Build (Debug)](https://ci.appveyor.com/api/projects/SoullessSentinel/cxbx-reloaded/artifacts/export/Debug.zip?branch=develop&job=Configuration:%20Debug&pr=false)

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
Chat on [Gitter](https://gitter.im/Cxbx-Reloaded/Lobby), or [Discord](https://discord.gg/26Xjx23).

## Contributing
We welcome contributions, large and small.

If you want to do some coding, be sure to read the [Developer notes](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki/Developer-notes).

Please contact us before you start working on something, so we can make sure your work is going to be accepted once finished.

### Prerequisites 
1. [Git for Windows](https://git-scm.com/)
2. [Visual Studio 2017](https://www.visualstudio.com/downloads/) (2015 might work, but is not officially supported)
3. Windows 8.1 SDK (Should be included with Visual Studio)
4. [Microsoft Child Process Debugging Power Tool](https://marketplace.visualstudio.com/items?itemName=GreggMiskelly.MicrosoftChildProcessDebuggingPowerTool)

### Fetching the code
Run the following command on the command line:
`git clone --recurse-submodules https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/`

Please note the `--recurse-submodules` parameter. This is required to fetch sub-modules.

Without this, Cxbx-Reloaded will fail to build.

If Cxbx-Reloaded was previously checked out without the submodules, they can be updated/fetched with the following command:
`git submodule update --recurse-submodules`

### Compiling
Simply open `build/Win32/Cxbx.sln` within Visual Studio, select your configuration (Release or Debug) and hit Build.

Please note that Debug builds are significantly slower, but include much more detailed kernel logging, and may help diagnose a problematic title.

## Support
You can support [Luke Usher](https://github.com/LukeUsher), initiator of Cxbx-Reloaded, on [Patreon](https://www.patreon.com/LukeUsher).

## Special Thanks
All contributors to the original Cxbx and Dxbx projects. Without them Cxbx-Reloaded would not exist at all.
