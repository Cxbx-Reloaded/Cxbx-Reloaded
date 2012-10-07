
Microsoft® DirectX® 8.0 SDK 

This CD is the Microsoft DirectX 8.0 Software Development Kit (SDK).  
It contains all DirectX software required to create DirectX 8.0 
compliant applications.  Parts of this SDK will also be made 
available for download at the Microsoft DirectX developer web page
(http://msdn.microsoft.com/directx).

Major components include:  DirectX 8.0 system components (runtimes);
DirectX 8.0 Application Programing Interface (API) Documentation for
each of the DirectX core areas (written for C++ or Visual Basic 
developers);  DirectX 8.0 headers & libraries;  Sample applications 
and source code;  miscellaneous tools and utilities.

After installing, those new to DirectX should start with the 
DirectX 8.0 documentation.  More seasoned developers may also want 
to view the "WHAT'S NEW" section.  Professional DirectX developers 
should refer to the "KNOWN ISSUES" section prior to raising concerns.

__________________________________________________________________


Contents:

- INSTALLATION
- WHAT'S NEW
- CD LAYOUT
- DIRECTX 8.0 SDK RELEASE NOTES AND KNOWN ISSUES 
- COMPILER SUPPORT

__________________________________________________________________


INSTALLATION:


ALWAYS UNINSTALL PREVIOUS RELEASES of the DirectX SDK prior to 
installing DirectX 8.0 SDK Release (see UnInstall).  The directory
structure and file names have changed significantly.  Installing on
top of a previous installation will cause you problems. 

On Windows 2000 systems, you must be logged in as administrator to
install the DirectX 8.0 runtimes. 

If you install the BDA cab, the DirectX 8.0 setup program requires 
that you restart your system.

UnInstall:

To uninstall, use "Add/Remove Programs" from the Control Panel to 
uninstall the InstallShield installations.

__________________________________________________________________


WHAT'S NEW:


DirectX Audio (DirectSound & DirectMusic)
=========================================

DirectX 8.0 audio covers both DirectSound and DirectMusic API's.  In 
DirectX 8.0, these API's are more closely aligned and tied together.

There are many new features.  They include:

   Effects processing on DirectSound Buffers
   DLS2 synthesis
        Arbitrary loop points
        Release waveforms
        Optional Filter per voice
        6-stage envelope
        Ramped volume scaling
        Time-stamped playback
   "Audiopaths":  a powerful and easy way to use the DirectMusic DLS2 synthesizer 
   for sound effects programming.
        Submixing of audio prior to 3D processing (by DLS2 synthesizer)
        Automatic streaming and ACM compression (ADPCM, etc.)
   Composition enhancements
   Audio Scripting

We also think you will want to look at using the DLS2 synthesizer as your 
main sound effects engine.  The DLS2 synthesizer is accessed primarily through 
the DirectMusic API's.  Please do not let the word, "Music" mislead you; the 
DirectX 8.0 DirectMusic API's have been enhanced *specifically* for sound effects
playback.  The new DirectMusic API's that are designed to support sound effects
refer to audiopaths. Please investigate audiopaths; they provide a lot of
cool features for sound effects.  For example, if you have multiple sounds 
that need to come from the same 3d location, you can pre-mix them (using Audiopaths) 
and use only one 3d hardware buffer to play them.

Parameter Validation:
DirectSound in particular has some greater parameter validation in place
when the DirectSound8 interfaces are used.

-Waveformats are more tightly parsed
-You can no longer create a DirectSoundBuffer with both CTRL_3D and CTRL_PAN set
-You can no longer create a DirectSoundBuffer with CTRL_3D set and a wave format
with more than 1 channel.  I.e. Only mono buffers may be created with CTRL_3D.

Scripting is very exciting.  It puts much of the control over the audio 
implementation of the game into the hands of the sound designer.  Using 
scripts, audio designers can change implementations, make the music 'magically' 
interactive, and play different sound effects based on in-game variables.  
It also drastically shortens the edit-integrate-listen cycle.  This should
result in improved sound effects and music, with much less game-programmer 
time required.

There are sections in the documentation on a new FullDuplex interface and
"Capture effects."  These interfaces are not currently supported.

If you are using DirectPlayVoice, you should use those interfaces.  If your
application requires audio capture outside of DirectPlayVoice, 
you should use the existing DirectSoundCapture interfaces.


Direct3D
========

Migration of DirectDraw into Direct3D:

In DirectX 8.0, DirectDraw® is fully integrated into Microsoft Direct3D
to enable drastically simplified application initialization, improved data 
allocation and management performance, and reduced memory footprint. 
The new Direct3D infrastructure provides the services that are called 
at relatively low frequency.  These services comprise:

- Creation of Direct3D
- Creation of resources (textures, vertex buffers etc)
- Display mode selection
- Presentation of rendered images to the display

New Graphics Features for DirectX 8.0 are:

Parallel vertex input streams for more flexible mapping to dynamic 
application data structures. Programmable vertex processing language 
enables programmers to write hardware shaders for:

- Morphing/tweening animation
- Matrix palette skinning
- User-defined lighting models
- General environment mapping
- Procedural geometry
- Any other developer-defined algorithm

Programmable Pixel processing language enables programmers to write 
hardware shaders for:

- Per-pixel lighting/bump mapping
- Per-pixel environment mapping
- Any other developer-defined algorithm

MultiSample Rendering support enables:

- Full-scene antialiasing
- Multisample effects such as motion blur, depth-of-field, etc.

Point Sprites enable:

- High-performance rendering of particle systems

3-D Volume textures enable:

- Range-attenuation in per-pixel lighting
- Volume atmospheric effects
- Easy texturing of very complex geometry

Higher-Order Primitive Support:

- For enhancing the appearance of 3-D content
- Facilitating mapping of content from major 3-D authoring tools

Higher-Level Technologies:

- 3-D content creation tool plugins for export into Direct3D of skinned 
  meshes using a variety of Direct3D techniques such as, multi-resolution 
  LOD geometry, and higher order surface data.

Direct3D Feature Notes:

No significant API changes are contemplated beyond this point.  
All implemented features have been tested individually 
to some extent, but they have not been tested in exhaustive combinations 
yet.  Many features in DirectX 8.0 are awaiting new hardware, however 
there is at least one implementation available for each of the features 
shipped:

- Programmable vertex shaders are supported on the host CPU. On CPUs they 
  will operate with performance comparable to HW implementations and can be 
  used immediately for DirectX 8.0 prototyping and development. 
- Programmable pixel shaders are supported on the host CPU using refrast 
  only. These can be used for prototyping DirectX 8.0 shaders for use when 
  hardware ships. 
- Parallel DMA data input is supported on refrast and the SW geometry pipeline.
- Point Sprites are supported in refrast, software T&L pipeline, and should
  be supported in a prototype DirectX 8.0 driver from at least one hardware 
  manufacturer.
- Volume Textures are supported in refrast. 
- Multisample rendering is supported in refrast and future hardware. 
- Higher-Order Primitives are supported in refrast and via D3DX utilities.  

D3DX
====

Debug Binaries for D3DX:

The D3DX library is a static library. However, to help debugging, we also 
have a debug only dynamic library of D3DX in DirectX 8.0. To use this, link 
with the d3dx8d.lib which is an import lib corresponding to the D3DX8D.DLL. 
If you are reporting a bug and sending Microsoft an example that reproduces 
that behavior, please make sure the executable is linked to the dynamic debug 
library.

The dynamic debug d3dx8 library is for debugging only and is not intended 
for use of shipping applications. D3DX8D.DLL is installed as part of the 
DirectX 8.0 SDK setup, not as part of the redistributable runtime setup. 
This will not be available as part of future operating systems.

If apps want to statically link to the debug d3dx8 library they need to 
link to d3dx8dt.lib. To statically link to a retail d3dx8 library please 
link to d3dx8.lib.

The D3DX Effect Framework

The D3DX effect framework allows you to define shading effects using a text 
file that describes the various techniques of implementing an effect. For 
an example of an effect file see water.sha in the C samples media directory.


DirectInput
===========

DirectInput for DirectX 8.0 features a major addition, the DirectInput 
Mapper. The Mapper makes it possible for applications to code to their 
in-game actions, without the effort of figuring out the actions represented 
on the device itself. Additionally, the Mapper makes user configuration 
of devices much simpler by offering a default configuration UI. 

DIPROP_KEYNAME is supported by the IDirectInputDevice8::GetProperty 
method. DIPROP_KEYNAME retrieves the localized key name, in the form 
of a DIPROPSTRING, from a DirectInput DIK_* value.

DIPROP_SCANCODE is supported by the IDirectInputDevice8::GetProperty 
method. DIPROP_KEYNAME retrieves the device scancode, in the form of a 
DIPROPDWORD, from a DirectInput DIK_* value.  


DirectPlay
==========

DirectPlay has been completely rewritten in DirectX 8.0 to provide the 
robust scalability required to build high performance, massively 
scaleable network games. With version 8.0, DirectPlay also provides 
voice communication via the DirectPlay Voice API.

Note: In these release notes the phrases "DirectPlay4", "DirectPlay4 
interface" or "DirectPlay4 session" are used to mean the IDirectPlay4 
interface or a session hosted using the IDirectPlay4 interface, even 
if that interface or session is implemented using the dlls that are 
part of DirectX 8.0. "DirectPlay8", "DirectPlay8 interface", and 
"DirectPlay8 session" mean the IDirectPlay8... family of interfaces,
or a session hosted using the IDirectPlay8... family of interfaces.

Since DirectPlay8 has been completely re-written from the ground up, 
the DirectPlay8 interfaces are not extensions of the existing 
DirectPlay4 interfaces. i.e. the DirectPlay8 interfaces do not 
inherit from the DirectPlay4 interfaces in the object oriented sense.

Also, DirectPlay8 sessions are not network compatible with DirectPlay4
sessions.

Note that the DirectPlay4 interfaces are still present in DirectPlay8
and are still compatible with previous versions of DirectPlay. So games
written for DirectPlay4 will still run on systems with DirectX 8.0 
installed.

To enable maximum scalability, DirectPlay8 uses a callback model. All
indications from DirectPlay8 are delivered to the application via an
application supplied callback function. Therefore DirectPlay can indicate 
multiple events to the application in parallel on different threads.

Additionally, performance critical operations such as Send and Receive 
can be performed without copying data, and without thread switching. 

This programming model allows DirectPlay applications to achieve maximum 
possible performance and scalability on both single-processor and 
multi-processor hardware.  

DirectPlay8 also adds:
  - Better support for NATs, Firewalls, and Proxy Servers
  - Application defined payloads on session enumerations and replies
  - Revised addressing scheme that is URL ruled based designed to be human 
    readable and portable
  - The use of I/O completion ports in Windows 2000
  - Improved ease of use through the removal of obtuse services, for example 
    groups within groups

The DirectPlay Voice interfaces allow you to quickly and easily establish 
voice communications over an existing DirectPlay8 or DirectPlay4 session. 

DirectPlay Voice provides the following features:
  - Peer to peer, forwarding server, and mixing server voice topologies
  - A variety of codecs from 1.2kbit/sec to 64kbit/sec, that can be used 
    royalty free in DirectPlay Voice.
  - Voice detection to automatically trigger voice transmission
  - Automatic gain control to automatically adjust the recording volume level
  - Adaptive queuing that provides a smooth speech stream with minimum latency
    over different and changing network conditions
  - Integration with DirectSound & DirectSoundCapture
  - Sound Hardware Test wizard
  - Support for IDirectPlay8 and IDirectPlay4 transport sessions

DirectPlay8 still has all the benefits of previous versions of DirectPlay, 
including:
  - session setup and teardown
  - session discovery
  - application launching
  - player management
  - group management
  - client/server and peer to peer operation
  - peer to peer host migration
  - bandwidth throttling protocol


DirectShow
==========

DirectX 8.0 marks the first release that DirectShow has been part of the
core DirectX redistribution. This means that for the first time,
applications using DirectShow need not install a separate redist, and 
current DirectX users can now take advantage of DirectShow in their
applications. Adding to the existing capabilities for audio and video
playback and capture that DirectShow provided, this new release also
provides the following new features:

Windows Media Format Support:  Two new filters enable DirectShow applications
to read and write files in Windows Media Format. The ASF Reader filter 
reads and parses Windows Media Format files. The ASF File Writer filter 
writes Windows Media Format files; it also does the necessary multiplexing 
and compressing. 

DirectShow Editing Services (DES):  This is a complete API for timeline and
switching services built on top of DirectShow. This API implements the
core of non-linear editing services, and allows for the creation of
composite media from a variable number of source audio and video streams. 
By allowing real-time effects and transitions, true "on-the-fly" source 
switching, run-time compositing, run-time seeking, and graph management,
DES provides a powerful high-level way to use DirectShow, while
still allowing applications to access lower-level DirectShow functions.
DES replaces cutlists, which are no longer supported.

New DVD Support:  Two new interfaces, IDVDControl2 and IDVDInfo2, greatly expand
the functionality of the DVD Navigator. The new MSWebDVD ActiveX® control 
makes this functionality available to script-based applications. 
New DVD features include: Karaoke multichannel audio support, audio volume 
control through IBasicAudio, support for frame stepping and frame-accurate 
seeking, simplified access to DVD text information strings, and improved support 
for parental management controls. 

DirectX Media Objects:  DirectX Media Objects (DMOs) are a new way to write 
data-streaming components. Like DirectShow filters, DMOs take input data and use 
it to produce output data. However, the application programming interfaces 
(APIs) for DMOs are much simpler than the corresponding APIs for DirectShow. As 
a result, DMOs are easier to create, test, and use than DirectShow filters.
A wrapper filter is provided to allow DMOs to be used within DirectShow. 

Media Parameters:   Media parameters are a new set of APIs that support run-time 
changes to an object's properties. They offer precise control over properties 
that change rapidly and require both high performance and repeatability. They 
enable a property to follow an exact curve, such as a sine wave or 
inverse-square curve, in real time.

New Filters:
- MJPEG Decompressor: Decodes a video stream from motion JPEG to uncompressed video 
- MJPEG Compressor: Compresses an uncompressed video stream using 
     motion JPEG compression. 
- Null Renderer: Discards samples without displaying or rendering the sample data.
     Applications can use this filter to discard data from a stream. 
- Sample Grabber: Retrieves media samples as they move through the filter graph.
     Applications can use this filter to obtain poster frames or other media data. 

Dynamic Graph Building:  DirectShow applications can now change the graph 
while it is still playing. This allows new audio or video streams to be added 
to a graph, effects to be inserted in the graph without any visible gap in the
playback, and chains of filters to be paused and run independent of the main graph.

Push Model Clock Support:  This allows for improved synchronization between
audio and video streams. The default audio renderer can now be slaved to
another clock, allowing for applications to deal with timing differences
between clocks on sound cards and capture devices, among other scenarios.

Graph Edit enhancements:  GraphEdit now displays filter registration information
in the Insert Filters dialog and allows you to spy on existing graphs in
external processes (if those processes register their graphs).


DirectMusic Producer
====================

This version of Producer features many new features to aid in both 
authoring and auditioning of DirectMusic content.  These features include 
authoring support for audio path configurations, which manages pchannel 
and synth assignments as well as effects routing, the new DLS level 2 
format (which includes stereo waves, layering of regions, and filtering) 
and a new API for scripting, which allows composers and sound designers 
to integrate audio content directly into an application.  Other major 
features include new track types, including a wave track, a script 
track, a secondary segment trigger track and a melody track, which 
integrates a variable approach with greater control of form. 


DirectX Control Panel
=====================
Developers can now toggle between DirectX retail and debug system components 
through the DirectX Control Panel. 


Broadcast Driver Architecture
======================

Broadcast Driver Architecture (BDA) is a set of Ring 0 and Ring 3 software 
components based on the DirectShow® architecture for streaming multimedia 
broadcast content. BDA is also a specification that defines how custom 
components, such as filters for parsing private data, interface with the 
rest of the system. Digital-TV applications can use the Ring 3 components 
to build filter graphs, implement a single model for tuning across all 
network types, automate the storage and retrieval of electronic program 
guide information, and capture IP data including ATVEF announcements, 
triggers, and packages.

__________________________________________________________________


CD LAYOUT:


The following is a brief description of the directories found with a complete
DirectX 8.0 SDK download.  Depending on options specified during
installation, some of these directories can be installed on your hard drive.

\Bin
    High level DirectX applications & tools.
    All can be accessed from the Start menu if "Utilities" are installed. 

    Note: Japanese systems will require Internet Explorer 5.0 (IE5) for
    the DirectX Control Panel to display properly.  IE5 can be found at
    http://www.microsoft.com/windows/ie/default.htm.
     

\Doc
    Contains reference documentation for the DirectX 8.0 APIs.  
    This Documentation must be viewed with Windows HTMLHelp.  

    Note:  The HTMLHelp viewer requires a minimum installation of 
    Internet Explorer 5.0 (IE5).  IE5 can be found at
    http://www.microsoft.com/windows/ie/default.htm.

\Essentls
    \DMusProd (Direct Music Producer)
       - DirectMusic Producer is the authoring tool for Direct Music.  It 
         allows composers and sound designers to use the interactive and 
         variable resources of DirectMusic along with the consistent sound 
         performance of DLS.  DirectX Audio Scripting can also be authored 
         and tested within Producer.  The DirectMusic Producer setup program 
         and all files are located here.
 
\Extras
   \Direct3D
      - Skinning exporter tools
   \DirectShow
      - Contains the DVDBoilerplate.  See readme.txt within the directory 
        for more information.
      - Contains additional media that can be used with DirectShow and the
        DirectShow Editing Services (DES) interfaces.
   \Documentation
      - Contains DirectX 7 HTMLHelp Documentation for English and Japanese.
      - Also contains DirectX 7 Documentation in Microsoft Word format
        (English only).
   \GameVoice
      - Contains the installer for the English Version of Game Voice Share, 
        which includes voice communication and game launching functionality. 
        For more information on Game Voice, visit http://www.gamevoice.com.
   \Symbols
      - Contains directories of DirectX 8.0 symbol files for 
        Win2000 and Win9x (retail and debug for each).

\Include
    contains DirectX 8.0 include files for DirectX core components.

\Lib
    contains DirectX 8.0 library files for DirectX core components.

\License
    Text versions of the DirectX SDK and End User License Agreements and 
    the Redistributable License Agreement.

\Redist
    Redistributable versions of the DirectX 8.0 Runtime.  

\Samples
    Contains all sample code and sample binaries.   Most samples can be
    accessed from the Start menu when installed via the downloaded 
    InstallShield setup.

\SDKDev
    This directory contains the runtime installs that are installed with 
    the SDK.  They are English only and contain both debug and retail 
    DirectX 8.0 system components that can be "switched" between retail 
    and debug without re-installing.  For the DirectX 8.0 components, 
    retail or debug can be switched via the DirectX Control Panel.

    The \retail folder contains the setup that will install non-DirectX 8.0
    retail components while the \debug folder will install all debug 
    components.  Both install DX8 retail and debug components.  These are 
    the only components that can be "switched".

__________________________________________________________________


DIRECTX 8.0 SDK NOTES AND KNOWN ISSUES
  

DirectX Audio (DirectSound & DirectMusic)
=========================================

Below are some known issues with DirectMusic and DirectSound in 
DirectX 8.0 that you may encounter.  

- ParamEQ and Distortion effects have parameters that are sampling rate 
  dependent.
  Setting these params to a value that is out of range for that sampling 
  rate will result in:
     Distortion:  Parameters are 'clipped' at the valid range for the 
                  sampling rate
     ParamEQ:  If the frequency is greater than can be supported by the 
               sampling rate, the effect is disabled-  In the DirectMusic 
               3DAudio sample, there may be some breakup if Doppler is set > 4
- DirectSound buffers that are created with DSBCAPS_CTRLFX must be at 
  least 100ms in length.


Direct3D
========

- All executables compiled against earlier DirectX 8.0 betas must be 
  recompiled with this version to run.
- Alt-enter in the samples results in texture corruption for some drivers. 
  Workaround: Get newer drivers.
- The value for MaxPrimitiveCount defines the max number of vertices
  as well as the max number of primitives that can be sent to a 
  DrawIndexedPrimitive or DrawPrimitive call.
- A bug in the Windows2000 kernel causes an issue upon mode-switches for 
  AGP surfaces locked with the NOSYSLOCK flag. The contents of any such 
  surface may  become inaccessible for the duration of the NOSYSLOCK lock 
  if a mode-switch happens after the Lock and before the Unlock. This happens 
  for both DX7 and DX8 applications. The most likely scenario is a DX8 
  application running on a driver that supports hardware vertex buffers, 
  but using software vertex processing (fixed or shader). Under these 
  scenarios, Direct3D might keep a NOSYSLOCK lock on an AGP vertex buffer. 
  If the user alt-tabs out of the application or does anything else to cause 
  a mode-change, the thread writing into the vertex buffer may fault. There 
  is no known workaround. Microsoft will fix this issue in the next version 
  of Windows and will attempt to ship a fix in a future Windows2000 
  Service Pack.
- A bug in the DX6 and DX7 implementation of 
  IDirectDraw*::GetDeviceIdentifier causes the DirectDraw runtime to write 
  four extra bytes past the end of the structure. To work around this issue, 
  allocate an extra four bytes at the end of your DDDEVICEIDENTIFIER* 
  structure. This bug does not exist in the IDirect3D8::GetAdapterIdentifier 
  version.
- The Direct3D8 run-time exhibits the following behvior on all Windows NT 
  platforms. Run a full-screen Direct3D8 application, press Ctrl-Alt-Del, 
  press ESC. The desktop will appear, but the display card will still be in 
  the application's resolution, not the proper desktop resolution. (The user 
  may alt-tab back to the application at this time.) A fix for this will be 
  in the next release of DirectX, and the problem will be fixed in the next 
  version of Windows NT. Developers are urged NOT to try to work-around this 
  problem nor code their own solutions, since such solutions are very likely 
  to interact poorly with our own fix.
- Terminating a process with un-released vertex buffers on Win9x DX7 APIs 
  can cause instability. This is caused by a bug in the automatic per-process 
  cleanup code in DirectDraw which causes VBs to be leaked permanently (until 
  reboot) on all Win9x platforms: Windows 95, 98, Millenirum. Applications 
  should be carefully coded to ensure all Vertex Buffers are correctly released
  before exiting the process.
- Applications should be careful when responding to the WM_MOUSEMOVE message. 
  If the application calls IDirect3DDevice8::SetCursorPosition with a coordinate 
  that is outside the current display mode size, then the Direct3D run-time may 
  cause another WM_MOUSEMOVE, which may cause the application to call 
  IDirect3DDevice8::SetCursorPosition again, resulting in an infinite loop. 
  The application should either be careful not to call 
  IDirect3DDevice8::SetCursorPosition  with invalid positions, or should never 
  call IDirect3DDevice8::SetCursorPosition  with repeated positions.
  One common scenario for generating invalid mouse cursors is to remove the 
  menu style from a window incorrectly when going full-screen. The SDK sample 
  applications incorrectly use SetWindowLong to remove the menu bar from the 
  window before going fullscreen. The correct technique is to call 
  "hOldMenu = SetMenu(hWnd, NULL)'" before going fullscreen, and then 
  "SetMenu(hWnd, hOldMenu);" before returning to windowed mode.
- When using the function ValidateDevice on Win9x the return value will be 
  DD_OK if it is successful.

Point Sprites:  

- When using the Direct3D software T&L pipeline (device created with 
  D3DCREATE_SOFTWARE_VERTEXPROCESSING flag set or with 
  D3DCREATE_MIXED_VERTEXPROCESSING flag and software vertex processing 
  toggled on), then point sprite support is guaranteed.  When using 
  hardware vertex processing, some older drivers may not support point 
  sprites, indicated by d3dcaps->MaxPointSize = 0.  Upgrading to a DX8 
  driver for these parts will enable them.

    To ensure consistent behavior for point sprite support, set
	D3DRS_CULLMODE = D3DCULL_NONE 
	D3DRS_FILLMODE = D3DFILL_SOLID
    when rendering point sprites.

Vertex Shaders:  

- Two vertex shader instructions may produce implementation-specific 
  results for out-of-range inputs, such as the log of a negative number.
  The log, logp and y- component of the lit instruction macro should not
  be passed negative values for this reason.

Driver/Device Specific Issues:

- The Windows Millennium driver for the ATI Rage 128 fails (draws nothing) 
  when using vertex shaders which output more than two texture coordinates 
  per texture coordinate set when the D3DTTFF_PROJECTED bit is not set.

D3DX
====

- This product includes software developed by the University of California,
  Berkeley and its contributors D3DX Mesh APIs.
- The ID3DXSkinMesh interfaces currently only support 16-bit mesh. 32-bit 
  mesh support is not yet implemented.
- All other D3DX Mesh interfaces do support 32-bit meshes even though the 
  docs say they do not.
- D3XGenerateAdjacency currently ignored the epsilon value and uses 0.0f 
  as the epsilon.
- In D3DXMatrixPerspectiveFOV, the aspect ratio is x/y instead of y/x.


DirectInput
===========

 - There is a bug in DirectInput which causes the Logitech Wingman Formula wheel to 
   report pedal data on the wrong axes.  
 - There are some know issues that can cause mouse behavior to be very sluggish 
   when debugging using DirectInput.   There are two reg files that will
   assist with this issue.  They are:
      Mouse and Keyboard Emulation On.reg
      Mouse and Keyboard Emulation Off.reg
   "Mouse and Keyboard Emulation On.reg" will force all mouse calls through 
   dinput emulation, thus making the mouse perform properly inside the debugger.  
   "Mouse and Keyboard Emulation Off.reg" will turn off force emulation.
   Only use these keys on development machines, they are not intended to be 
   redisted as they are a global setting that effects all Direct Input clients.
 - The following devices are supported with device images in this release.    
      + ACT LABS RS
      + ACT LABS Force RS
      + Gravis Xterminator Dual Control	
      + Gravis Blackhawk Digital	
      + Gravis Gamepad Pro USB
      + Gravis Gamepad Pro
      + Gravis Eliminator Gamepad Pro
      + Gravis Xterminator Digital GamePad
      + InterAct/3dfx SV-262A HammerHead Fx
      + InterAct Axispad
      + InterAct SV-251 Raider Digital
      + Logitech WingMan Formula (Yellow)
      + Logitech WingMan Extreme Digital 3D
      + Logitech WingMan Gamepad Black (USB)
      + Logitech Wingman Rally Force USB
      + Logitech Wingman Rumblepad
      + Microsoft Sidewinder Gamepad Pro
      + Microsoft SideWinder Force Feedback Wheel
      + Microsoft SideWinder Force Feedback Pro
      + Microsoft Sidewinder FF 2
      + Microsoft Sidewinder Dual Strike
      + Microsoft Sidewinder Precision 2
      + Microsoft Sidewinder Plug and Play
      + Microsoft SideWinder Game Pad
      + Microsoft SideWinder Precision Racing Wheel
      + Microsoft SideWinder Precision Pro
      + Microsoft SideWinder Freestyle Pro
      + Microsoft SideWinder 3D Pro


DirectPlay
==========

The following are known issues with DirectPlay in this release that 
you may need to work around.

- Note that the dwTimeoutUntilKeepAlive member of the DPN_CAPS structure 
  specifies a minimum value. The first keepalive packet may be sent as 
  much as 30 seconds later than the minimum value specified in this 
  member. For example, if you set dwTimeoutUntilKeepAlive to 60000 
  (60 seconds) the first keepalive packet may be sent anywhere from 
  60 to 90 seconds after the last normal packet was sent.
- The "debug output level" slider on the DirectPlay tab of the DirectX 
  control panel is not functional. For instructions on how to change the 
  debug output level, see the "dp8log.txt" file in the 
  [sdkroot]\bin\DXUtils directory.
- To use DirectPlay Voice in full duplex mode, we recommend that you update
  to the latest drivers for your audio device. Some older drivers do not
  provide reliable full duplex performance.
- If you connect to a location that you have enumerated, the enumeration 
  will be canceled automatically for you.  If you attempt to do a cancel 
  asynchronous operation on this enum after the connect is completed you 
  will get back DPNERR_USERCANCEL error code.  Be aware that this is the
  expected behavior and that you should not exit with an error condition.
- The modem service provider implementation for DirectPlay8 requires 
  TAPI 2.0 functionality.  TAPI 2.0 is available as an installed component 
  on Windows 98 and later, but not on Windows 95.  If your application 
  requires modem functionality on the Windows 95 platform, the TAPI 2.0 
  patch for Windows 95 can be obtained on the Microsoft FTP site 
  (ftp://ftp.microsoft.com/developr/TAPI/tapi2195.zip) and distributed 
  with you application.

Notes on NATs (Network Address Translators):

- There are many different NAT solutions available, some of which are
  different from others. We have attempted to document some of the issues
  with various NAT solutions, but is not comprehensive.  Please refer to 
  the DirectX Developer Page at http://msdn.microsoft.com/directx for 
  further information on these issues.

DirectPlay Voice codecs via Windows ACM (Audio Compression Manager):

- DirectPlay Voice uses the following codecs via the Windows ACM:
   - Microsoft ADPCM
   - Microsoft GSM
   - DSP Group Truespeech
- Installation of these codecs with Windows is optional. If they 
  are not installed, then one or more of the compression types in 
  DirectPlay Voice will not function. If the application attempts 
  to use a compression type that is not functional due to a missing 
  ACM codec, it will receive the DVERR_COMPRESSIONNOTSUPPORTED 
  return code.
- On Windows 2000, the user can install these codecs through the 
  Add/Remove Hardware wizard. The user should select 
  "Add/Troubleshoot a device". When a list of hardware is presented, 
  the user should select "Add a new device", then on the next 
  page select "No, I want to select the hardware from a list". 
  This will bring up a list of hardware. Select "Sound, video, and 
  game controllers". The "Audio Codecs" item is listed under 
  "(Standard system devices)". 
- On Windows 9x, the user can install these codecs through the 
  Add New Hardware wizard. When prompted, the user should select 
  "No, I want to select the hardware from a list". In the list of 
  device types, codecs are listed under "Sound video and game 
  controllers". The codecs are listed under two manufacturers. 
  The ADPCM and GSM codecs are listed under "Microsoft Audio 
  Codecs" and the Truespeech codec is listed under "DSP Group".


DirectShow
==========

- The GraphEdit Tool (and some of the DirectShow samples) requires MFC42.DLL
  to be present on the system.  The DirectX SDK Setup does not install this 
  DLL.  This will only be an issue on Windows 95 systems and will require you
  to install Microsoft Visual C++ to acquire the DLL.
- Application development using the DirectShow base classes libraries requires
  installation of the DirectShow samples.  The source code for the base classes
  is now included as a sample project, rather than as a separate Classes\Base 
  directory.  You must compile these base classes to generate strmbase.lib and 
  strmbasd.lib, which are no longer installed as binary files.   If you are
  linking with strmbase.lib or strmbasd.lib with your own application, be sure
  to build and link with the updated DirectX 8 libraries (instead of the older
  libraries that are installed with Visual C++ or the Microsoft Platform SDK).
- The samples included with the SDK are configured to compile and link with 
  the updated libraries, which are generated by building the BaseClasses project.  
  Further build instructions are provided in:
	  <DXSDKInstallFolder>\samples\Multimedia\DirectShow\readme.txt.
- The <streams.h> header file, along with many other DirectShow-specific header
  files, is now located in the DirectShow samples BaseClasses directory, 
  which includes the BaseClasses source code and header files.  You must add 
  the BaseClasses directory to the beginning of your project's include path 
  settings in order for Visual C++ to find all of the updated header files.
- To use the Teletext Decoder Fonts that are shipped with DX8, make sure that
  the WST_* fonts are installed by examining the \windows\fonts directory.
- There is a known problem with the MP3 decoder filter when used with
  Audio Effects DMOs.  If you add an audio effect DMO (like WavesReverb, Echo,
  or Flange) to an existing MP3 filter graph, the audio could be played back
  at double speed.  The problem would occur whenever the MP3 decoder is connected 
  to a downstream audio effect dmo (via the dmo filter wrapper) that offers a 
  default audio input type which doesn't match the native format of the 
  uncompressed MP3 stream.  For example, if you render a 22kHz MP3 file and 
  add a DMO Audio effect, the MP3 filter could misinterpret the input type as
  being 44kHz when it is actually 22kHz.  This would not be a problem for 44kHz 
  MP3 files.  This issue will be fixed in a future release.
- If you have the SDK on CD-ROM, the Extras\DirectShow\Media  directory 
  contains additional media that can be used with DirectShow and the DirectShow
  Editing Services (DES) interfaces.  We have included DV-encoded AVI videos, 
  larger MPEG videos, and higher-quality JPEG images.  Since the DV files are 
  very large, you should copy them to your hard disk before playing them to 
  ensure the best performance.
- Remote graph spying with GraphEdit works only on Windows 2000.
- The DirectShow Editing Services (DES) Color Generator source filter may 
  generate blue or green components incorrectly if they have small values.  
  For best results, use solid primary colors or set Blue and Green 
  components to values of either 0xFF or 0x00.


Broadcast Driver Architecture
======================

Known issues with Broadcast IP Data Reception under Windows 2000:

- After rebooting a Windows 2000 system with a BDA receiver card installed,
  the BDA NDIS IP NIC is not immediately active.  It becomes active only 
  when an application inserts the BDA IPSink filter into a graph and connects
  its input pin.  Once activated by connecting the IPSink filter's input pin,
  it takes 20 to 30 seconds for the BDA NDIS IP NIC to be assigned an IP 
  address.  The address assigned will be a Net 10 address.  Once activated, 
  the BDA NDIS IP NIC will remain active until the system is rebooted.
- There may be some applications that use the BDA NDIS IP NIC as the default 
  LAN adapter.  These apps will fail if the BDA NDIS IP NIC is active.  To 
  de-activate the BDA NDIS IP NIC, reboot the system and do not run a TV graph 
  or application.


Documentation
=============

- For proper viewing of the DirectX Documentation, install the docs from
  the DirectX 8.0 SDK CD.  Specify a full installation, or if you chose a 
  Custom install, ensure that you've selected Documentation.  
- The known issues with the DirectX 8.0 Documentation are written up in a
  separate file.  You will find these issues listed in the 
  DirectX8_Doc_Readme.txt file found in the <DXSDKCDRoot>\dxf\doc\DirectX8 
  folder.  This file is also installed into the <DXSDKInstallFolder>\doc\DirectX8
  when DirectX8 Documentation is installed.
 

Samples 
=======

Miscellaneous Issues:
 
- Some samples will compile with errors with the UNICODE flag defined.
- Visual C/C++ 5 compatibility in DirectX 8.0 depends on Service Pack 3 
  for Visual Studio  97. If you see the following errors:
    "fatal error LNK1106: invalid file or disk full: cannot seek to 0x389bc8bf"
    "Internal error during Pass1"
  then you need to upgrade your Visual C/C++ 5 installation. Service packs 
  for Visual Studio can be found at http://msdn.microsoft.com/vstudio/sp/vs97.

Direct3D Samples:

- There are miscellaneous issues with some display hardware & drivers.
  Please let us know of any problems you encounter and specify whether you
  believe it is sample or driver related.

DirectShow Samples:

- There are several .XTL sample files in the \media folder.  These files 
  demonstrate some of the functionality available in the DirectShow Editing 
  Services.  You can render these files using the XTLTest C++ sample and 
  the XTLTestVB and DexterVB VisualBasic samples.  All provided XTL files 
  refer to media that ships in the SDK's media directory.  Rendering XTL 
  files is a great way to get an idea of what the new DirectShow Editing 
  Services APIs make possible.  For a simple demo, drag a few of the SDK 
  media directory's sample .XTL files onto the XTLTestVB sample's client area.
- The DirectShow Visual Basic samples demonstrate more of the DES functionality 
  than the C++ samples.  Be sure to run SlideShowVB and TrimmerVB if you are 
  interested in DES.  You can also use the DexterVB ("Dexter Explorer") 
  application as an XTL development/test tool.
- The Texture3D sample is known to fail on some systems (referencing
  invalid memory), for which you may see an error message.  This failure is
  in initialization of D3D.  We recommend updating your video card drivers
  to the most current revision, as that usually resolves the problem.
- The DirectShow IDL files are currently located in the
  \include\DShowIDL directory of the SDK for your convenience. 
- You will be required to build the \BaseClasses libraries before any sample
  that depends on them can be built or executed.  Please review the Readme.Txt 
  file in the DirectShow samples root directory for more information on 
  building the samples.


DirectX for Visual Basic Samples:

- DBMON.exe is a console application which allows the Visual Basic DirectX 
  developer to take advantage of extended output that debug installs of 
  DirectX provide. This application works only under Windows 2000. 
  To use, execute the application. All debug output will be captured and sent 
  to the console window. If you are not seeing debug output, ensure that 
  you have installed the debug version of DirectX, and that you are running 
  Windows 2000.
- DXVB Messenger Server is the server portion of a client/server instant 
  messaging application.  This sample requires the use of ActiveX Data Object
  (ADO) to maintain the data needed.  If you are running Windows 9x, you must 
  have the latest version of ADO installed on your machine to compile or run 
  this sample.  You can install the latest ADO while Visual Basic Professional 
  is being installed or you can download the latest ADO from the web at:
  http://www.microsoft.com/data/ado/.  ADO is installed by default with 
  Visual Basic 6 Enterprise.
- If you are experiencing problems searching for and connecting to sessions 
  using DirectPlay for VisualBasic, you may need to update your system.  
  To update, install DCOM98 from:
      http://download.microsoft.com/msdownload/dcom/98/x86/en/dcom98.exe 


Other
=====

- The DirectDraw "Debug Level" slider in DirectX Control Panel Applet does 
  not function.   Instead use the "Debug Output Level" slider on the Direct3D 
  tab, this will effect both DDraw and D3D8.   Also note that DirectDraw labels
  it's debug spew as coming from "Direct3D8".
- The DirectX runtime will not operate on NEC PC98 systems.  SDK and Runtime 
  installations have been intentionally blocked on this system.
- The DirectX 8.0 runtime requires a Pentium compatible CPU for operation.  
  The DirectX runtime setup will not install on processors that do not report 
  Pentium compatibility.  Some older Cyrix processors, while compatible with 
  the Pentium instruction set, report as x486 processors, and thus do not 
  support installation of the DirectX 8.0 run time.
- When installing the DirectX 8.0 runtime from the DirectX SDK you will 
  notice that the size requirements are inflated from previous DirectX versions.
  For example, if you were only to install a DirectX 8.0 runtime from the 
  DirectX SDK onto a system with low disk space, you would see that at least 
  120 MB is required.  This is because both the retail and debug runtimes are
  included in each of the DirectX SDK runtime installs (this allows developers
  to switch between retail and debug without having to re-install).  However, 
  this inflated size is not true of the redistributable runtimes shipped with
  the SDK.  Redistributable installs will require significantly less disk space.
  While this number is dependent on the disk format (Fat16/32 vs. NTFS) we 
  expect approximately 40 MB of free space is required for uncompression and 
  installation and approximately 15 MB once installed (and this is only true for
  systems with no previous DirectX version installed).  The DirectX Setup API 
  will check for and display error messages when these requirements are not met.
- Windows 95 Japanese Systems will require MS UI Gothic patch for Windows 95 
  or Internet Explorer 5.0 (or after) for the DirectX Control Panel to display 
  properly. MS UI Gothic Patch can be found at 
  http://www.microsoft.com/JAPAN/win95/msgotupd/default.htm and IE5 (or after)
  can be found at http://www.microsoft.com/windows/ie/default.htm.

__________________________________________________________________


COMPILER SUPPORT

All DirectX 8.0 SDK samples are developed and compiled with Visual Studio 6.0.  
We have provided Visual Studio 6.0 DSP files for your convenience.  

All DirectX 8.0 libraries, with the exception of the DirectShow libraries, should
be compatible with Visual Studio 5.0 SP3 or higher.  

__________________________________________________________________


SPECIAL THANKS

Marlin Studios

Textures used in the Billboard sample, tree02S.tga, tree01S.tga & tree35S.tga,
are provided courtesy of Marlin Studios (http://www.marlinstudios.com).  
They are provided for use with this sample only and cannot be distributed 
with any application without prior written consent.  

Viewpoint Digital, Inc.

The model used by the Visual Basic AutoParts sample, engine1.x, is provided 
courtesy of Viewpoint Digital, Inc. (http://www.viewpoint.com).  It is provided 
for use with this sample only and cannot be distributed with any application 
without prior written consent.  V6 Engine Model copyright 1999 Viewpoint 
Digital, Inc..

3DCAFE and Platinum Pictures

The model in the Dolphin samples, Dolphin.x, is derived from the model files 
(dolphins.zip) provided courtesy of 3DCAFE and Platinum Pictures  
(http://www.3dcafe.com/).  It is provided for use with this sample only.  
For further terms of use please visit the 3DCAFE site or contact 3DCAFE.
