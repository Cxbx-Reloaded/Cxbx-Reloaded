# Cxbx-Reloaded - Original Xbox Emulator 
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://img.shields.io/badge/License-GPL%20v2-blue.svg)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/iao43irxl3umbp33?svg=true)](https://ci.appveyor.com/project/SoullessSentinel/cxbx-reloaded)
[![Gitter](https://badges.gitter.im/gitterHQ/gitter.svg)](https://gitter.im/Cxbx-Reloaded/Lobby)

Cxbx-Reloaded is an emulator for running Microsoft Xbox (and eventually, Chihiro) games on Microsoft Windows.

The project began its life as a fork of Cxbx, with added 64-bit support. Work is currently underway to backport some of the improvements made from Dxbx.

Cxbx-Reloaded is early in development and still pretty unstable, so don't expect it to run much at this stage.

## System Requirements
### Minimum
  * OS: Windows 7 64-bit or newer. 32-bit installations are not supported.
  * Video card: anything that supports Direct3D 8.
### Prerequisites
  * Visual C++ 2015 and 2017 redistributables may be required. Download them [here](https://support.microsoft.com/en-gb/help/2977003/the-latest-supported-visual-c-downloads).
  
## Automated Builds
Cxbx-Reloaded is not yet ready for general release, but the latest development build can always be downloaded from our [AppVeyor](https://ci.appveyor.com/project/SoullessSentinel/cxbx-reloaded/branch/master) (navigate to Configuration: Release => Artifacts => export\Release.zip).

## Compatibility
We have a basic [compatibility list](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/222). If you have found an issue that has not been reported, please create a report (see below) for each issue per game rather than commenting on the list!

## Bug Reports
As Cxbx-Reloaded is in early stages of development right now, most games will either crash or do nothing. Please do not submit issues for these games.

Please report games that actually do something. If a game outputs graphics or audio before crashing, please submit a bug report at [GitHub](https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues) containing the build tested with, error message displayed (if any), screenshots, an Xbe dump (created via [Edit -> Dump Xbe Info To -> File]), and a Kernel Debug file (created when running a game when [View -> Debug Output (Kernel) -> File]) is selected.

## Additional information
Cxbx-Reloaded has a [wiki](http://github.com/Cxbx-Reloaded/Cxbx-Reloaded/wiki) containing various subjects and background information.
Chat on [Gitter](https://gitter.im/Cxbx-Reloaded/Lobby) or [Discord](https://discord.gg/26Xjx23).
You can find a screenshot gallery at [imgur](http://imgur.com/a/Bzvti).

## Support
You can support Luke Usher (initiator of Cxbx-Reloaded) on [Patreon](https://www.patreon.com/LukeUsher).

## Special Thanks
All contributers to the original Cxbx and Dxbx projects, without which Cxbx-Reloaded would not exist at all.
