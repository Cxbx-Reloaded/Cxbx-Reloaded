Controller Input is configured using a dialog inside
EmuDInput.cpp/.h

Configuration is actually saved into both the registry
and shared memory within Cxbx.dll. This means that all
active emulated games (even if there are many of them
at the same time) will have the ability to refresh the
current configuration at any given time.

This works out really great because this means you can
make changes in the input configuration at any time and
immediately see the changes take affect.

It is important to note that all access to this shared
memory, as with all shared memory, should be protected
with a mutual exclusion mechanism. Win32 provides such
a mechanism and it will be used.

-caustik