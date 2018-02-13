using System;
using System.Runtime.InteropServices;

namespace VsChromium.Core.Win32.Debugging {
  [StructLayout(LayoutKind.Sequential)]
  public struct CREATE_THREAD_DEBUG_INFO {
    public IntPtr hThread;
    public IntPtr lpThreadLocalBase;
    // x1nix: Alignment fix, as PTHREAD_START_ROUTINE is failing to Marshal
    public IntPtr lpStartAddress;
  }
}