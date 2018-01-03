# CxbxDebugger

CxbxDebugger is an experimental tool to aid debugging Xbox executables from within Cxbx.

The goal is to resolve XBE symbols using the function signatures identified by Cxbx-Reloaded.

This is a workaround for patching or creating PDB files at runtime for Cxbx-Reloaded (where the memory region is pre-allocated - see `virtual_memory_placeholder`). However, in future it may be viable to supporting loading symbols from matching PDB files.

## Methods

The debugger is a standalone C# application which wraps the child instance of Cxbx-Reloaded used to launch the XBE file.

It uses the debugger API provided by Windows, primarily **WaitForDebugEvent** and **ContinueDebugEvent**. The C# implementation is taken from the VsChromium project.

## Roadmap

### Primary

* ~~Create wrapper for the Cxbx-Reloaded child process~~
* Support new threads
* Support displaying of interrupts and exceptions
* Support resolving relevant XBE symbols from a callstack

### Secondary

* Suspending threads and checking memory
* Breakpoints - inserting and handling interrupts

## Thanks

* The Chromium Project for [@VsChromium](https://github.com/chromium/vs-chromium), specifically [these Win32 sources](https://github.com/chromium/vs-chromium/tree/master/src/Core/Win32)

* Sebastian Solnica ([@lowleveldesign](https://github.com/lowleveldesign)) for [Process Governor](https://github.com/lowleveldesign/process-governor) and [MinDBG](https://github.com/lowleveldesign/mindbg)

* Alexander Bothe ([@aBothe](https://github.com/aBothe)) for [DDebugger](https://github.com/aBothe/DDebugger)
