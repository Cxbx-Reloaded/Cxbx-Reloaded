cxbx website: http://www.caustik.com/xbox/

version: 0.7.8c (09/02/03)
--------------------------------

- Spontaneous CreateDevice failures fixed (many, at least)

- Exe now able to generate into temporary directory, allowing
  games to be run from a read only device without path issues.

- Finally, Cxbx.dll and Cxbx.exe enforce version synchronization.
  This should at least diagnose about 9 billion people's problems.

- Very minor splash image tweaks

version: 0.7.8b (08/30/03)
--------------------------------

- Fixed debug messages accidentally left in the previous release.

version: 0.7.8 (08/29/03)
--------------------------------

- Halo executes (no graphics yet)

- Overlays simulated on PCs that do not support them in hardware.

- YUY2 overlay capabilities detection improved significantly

- Fixes to mesh rendering (thanks kingofc!). The "XRay" XDK demo is
  extremely impressive now (and runs very efficiently), as well as
  the Gamepad/Rumble demos.

- Z: drive simulation repaired

version: 0.7.7b (07/16/03)
--------------------------------

- Fixed lost compatibility with X-Marbles, etc

version: 0.7.7 (07/15/03)
--------------------------------

- Turok Evolution displays startup graphics and intro sequence!!

- Stella and a few other homebrew games are now playable :]
  This means you can play all your atari games on Cxbx, which
  is a great novelty.

- Finally low level emulation of the heap, which is a very
  very nice thing and fixed some glitches/bugs.

- Fixed timing (FPS is much higher now! it is unbelievable)

version: 0.7.6 (07/07/03)
--------------------------------

- Many more homebrew apps show some graphics

- DirectInput bugs fixed

- PointSprites works great without source hack

- Lots of code cleanup

version: 0.7.5 (06/30/03)
--------------------------------

- X-Marbles homebrew demo is playable!

- New GUI bitmap (thanks, bot!)

- DirectSound emulation has begun.

- Corrected converted Exe stack commit.

- Fixed a really annoying problem with the debugger
  not being able to attach, that has been around for
  way way too long.

- Caught up 4627 pretty far.

- PointSprites and Gamepad demos, and some other new
  XDK samples run better.

- Added more Direct3D/XAPI emulation, Hunter The Reckoning
  gets pretty far but no graphics yet.

version: 0.7.4 (06/23/03)
--------------------------------

- FIRST RETAIL GAME GRAPHICS (simple, but yay!!)

- Quad rendering

- Tons of new XD3D emulation

- Some nice demos play now, including the cool PointSprites
  demo. There are some issues right now with the mipmap filters

- Fixed a user input bug (was ignoring digital buttons)

version: 0.7.3 (06/18/03)
--------------------------------

- Meshes

- Indexed [primitive/vertex] rendering

- Fixed *part* of the invisible texture problem

version: 0.7.2 (06/13/03)
--------------------------------

- Textures (bmp, jpg, png, partial .xpr, etc)

- rtinit/cinit run on lower level

- stdio seems to be working great :]

- some advanced texture stuff (TCI)

version: 0.7.1 (05/30/03)
--------------------------------

- Video Configuration!

- Fixed some Xbe change detection logic

- Open Xbe and Import Exe can be done when a file
  is already open (automatically closes, checking if
  you made changes first).

- Direct3D Lighting is working!

version: 0.7.0 (05/27/03)
--------------------------------

- Magically shrunk cxbx.dll/cxbx.exe file sizes enormously!

- Added Controller Input and Configuration!

- Added Recent Xbe/Exe file menus

- Support __declspec(thread) style TLS

- Fixed GUI color issues

- Massive code re-organization.

- Sooo many random optimizations

- Went lower level with certain components of
  emulation. Progress should speed up.

- Some Xbe parsing and debug output fixes.
  You should now be able to deal with the
  slightly odd Linux Xbe files.

- Much better emulation exception handling

version: 0.6.0-pre12 (02/23/03)
--------------------------------

- HLE has advanced to intercepting Direct3D and
  Xapilib calls. A simple Xbox app built with a
  4361 or 4627 XDK has been shown to work.

version: 0.6.0-pre11 (02/09/03)
--------------------------------

- HLE has begun. Cxbx is capable of emulating a
  blank XDK project. That is, if you had the XDK
  and created an Xbox "game" that did absolutely
  nothing, Cxbx could run it :P. Sounds pointless,
  but it is actually very significant

- New icon! Pending approval for use by the author

version: 0.6.0-pre10 (02/07/03)
--------------------------------

- Tons of debug console output when opening, or
  converting, or saving xbe and exe files. This
  was very easy to add because of the flexible
  new debug console technique.

version: 0.6.0-pre9 (02/06/03)
--------------------------------

- Released source code under GNU license.

- Debugging interface changed. Much cleaner.

version: 0.5.2 (12/14/02)
--------------------------------

- Fixed a bug in section name generation. This might
  add some compatibility, not sure yet.

version: 0.5.1 (??/??/??)
--------------------------------

- more .xbe information added to core and xbe dump

version: 0.5.0 (11/16/02)
--------------------------------

- fixed a bug in displaying section digests.

- added conversion from .exe to .xbe!!

- more code cleanup, tiny ui improvements.

version: 0.4.4 (11/01/02)
--------------------------------

- updated .xbe structure for more acccuracy

- added/fixed alot of information in xbe info
  dumps. most notably is the TLS information, 
  which is finally completely correct.

- lots and lots of new kernel function prototypes
  and structs/enums are very accurate now.

version: 0.4.3 (10/09/02)
--------------------------------

- added edit menu options to patch for allowing
  more than 64mb of ram, and also to toggle between
  debug mode / release mode. i also fixed a few 
  relatively minor gui things, such as suggesting
  an appropriate name for saving an .xbe file, instead
  of just defaulting to "default.xbe".

version: 0.4.2 (10/07/02)
--------------------------------

- finally got around to adding logo bitmap import
  feature. this is pretty damn cool because you
  can change that little logo that appears when
  you boot your xbox software to whatever you want
  it to be. for example, you can modify xbox media
  player to display "XBMP" instead of "Microsoft"

version: 0.4.1 (10/04/02)
--------------------------------

- internally alot of little things have changed,
  code is organized pretty well now. software run
  through the emulator typically safely terminates,
  which is pretty damn cool from my perspective.

version: 0.4.0 (BETA) (09/16/02)
--------------------------------

- total code rewrite. most the funcionality
  has remained intact with cleaner code UI
  and code design.

- logo bitmap is now decoded and displayed
  in the main window when you open an .xbe
  file. debug output window traces kernel
  calls. logo bitmap can be exported to a
  bitmap file.

- xbe_info.txt now displays the correctly
  decoded kernel thunk table address.

version: 0.3.1 (09/02/2002)
--------------------------------

- significantly decreased file sizes for cxbx.exe
  and cxbx_krnl.dll. Also made debug output cleaner.

version: 0.3.0 (08/19/2002)
--------------------------------

- various gui changes, new web site, significant
  changes in emulation theory. kernel exports are
  now hijacked and interpretted.

version: 0.2.2 (07/24/2002)
---------------------------

- fixed various minor GUI problems

- added kernel thunk address description in GUI

version: 0.2.1 (07/24/2002)
---------------------------

- added "Convert To .EXE" menu option.

- fixed problem where entry point wasn't being detected
  correctly when converting to .exe

version: 0.2.0 (07/24/2002)
---------------------------

- drastically changed UI, took out convert to .exe
  feature (temporarily).

version: 0.1.3b(07/18/2002)
---------------------------

-  oops..fixed an incorrect debug xor value

version: 0.1.3 (07/16/2002)
---------------------------

- .xbe file information dump now shows retail/debug
   translated addresses.

version: 0.1.2 (07/16/2002)
---------------------------

- cxbx now dumps .xbe file information in a .txt file.

- new icon ?

- various tiny improvements