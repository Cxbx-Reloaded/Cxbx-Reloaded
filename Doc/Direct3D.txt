Direct3D resources are going to be a little bit of a hack.
There is a tough situation, because precompiled resources
(XPR) can be loaded into memory manually by an Xbox Game,
and then "Registered" with pResource->Register(addr);

This is a problem because we never have an opportunity to
intercept the "this" pointer for this datatype. The base
resource class looks like this:

DWORD Common;
DWORD Data;
DWORD Lock;

The first idea is to tuck away the PC Direct3D resource
pointer inside of pResource->Data. The trouble with this
is that some Xbox Games will directly access and modify
this member.

Another technique is to hide the PC Direct3D resource pointer
inside of the buffer allocated by Data. This could work, as
long as Xbox Games do not access the resource Data after the
initial Register function is called.

The currently used method is to tuck the pointer away inside
of the Lock member, and hijack any functions that attempt to
access the Lock member variable.
