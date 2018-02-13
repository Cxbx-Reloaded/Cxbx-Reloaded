using System;
using System.Runtime.InteropServices;

namespace VsChromium.Core.Win32.Debugging {
  [StructLayout(LayoutKind.Sequential)]
  public struct CREATE_PROCESS_DEBUG_INFO {
    public IntPtr hFile;
    public IntPtr hProcess;
    public IntPtr hThread;
    public IntPtr lpBaseOfImage;
    public uint dwDebugInfoFileOffset;
    public uint nDebugInfoSize;
    public IntPtr lpThreadLocalBase;
    // x1nix: Alignment fix, as PTHREAD_START_ROUTINE is failing to Marshal
    public IntPtr lpStartAddress;
    public IntPtr lpImageName;
    public ushort fUnicode;
  }
}