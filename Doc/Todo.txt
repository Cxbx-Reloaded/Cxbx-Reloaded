Cxbx Todo (* denotes high priority, + denotes medium priority)

  * Go lower level with mutants (maybe unnecessary), use \??\ and also
    set root directory to null.

  * DirectSoundUseFullHRTF -> 0x192D8C (Halo)..too small to detect?

  * Palette needs to actually be used, not just allowed to execute.

  * Check if 4361 Resource8_Release is the same as 4627 & 3925 (3911)

  * When focus is lost, enumerate and pause all threads, aside from the
    user input thread (and any other critical threads). alternatively,
	stash all of the thread handles into a list and suspend them all when
	necessary.

  * Configurable dead zone, for touchy controllers

  * Push Buffer Size is stored in a global. This may be important (hard to tell)

  * Perhaps for global variable detection, have some sort of array of
	function + offset pairs to check. That way if an Xbe happens to not
	use a particular function (wont get linked in), there are other chances
	to locate the variable. In the even that none are found, the global ptr
	should be set to NULL, and any references to it should not assume it
	will not be NULL.

  * Special temporarily X_D3DResource handles need to be kept track of and
    periodically garbage collected. Garbage collection frequency can be a
	core configuration option.

  * Register() probably needn't re-unswizzle each time. cache a copy of
    the address it was registered to (or a global table) and simply update
	the data pointer to that IDirect3DTexture8 instance.

  * Support cards that just plain dont have 32 bit color?

  * The notorious Heap Allocation crash still needs to be solved on a low
    level. It is strongly preferable to debug this instead of having to 
	intercept the entire Rtl heap set of functions.

  * Stabilize TLS (it's close...)

  + Closing a console should not terminate the entire process.

  + Perfect the timing on KeTickCount. This can be updated along with the
    Xbox-Never-Sleeps, which would give much higher precision than the
	current seperate-thread method.

  + Some sort of delete-after-emulation type of functionality?

  + Use SetDataFormat instead of parsing device input by hand?

  + Batch config all buttons (should be very easy..just click one by one)

  + Configuration screens may not necessarily need to be modal windows.

  + Xbe file associations via user configuration. This could include an
    option to either automatically execute the Xbe, or simply open it up
    in Cxbx's main window. For this purpose, Cxbx.dll should also be
    registered in the system so that a converted Exe can run from anywhere.

  Converted Exe files should use the Cxbx icon

  When loading a file, menus and WM_CLOSE should be disabled and an update
  progress should be sent via callback from core.

  Encapsulate RecentFiles into a nice little class

  Allow a logo bitmap to be added if one does not exist. This may require that
  the size of headers be increased. (sizeof_headers).

  Xbe::m_Header should be allocated dynamically to make room for huge headers.
