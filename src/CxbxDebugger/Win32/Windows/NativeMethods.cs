using System;
using System.Runtime.InteropServices;

namespace LowLevelDesign.Win32.Windows
{
    public static class NativeMethods
    {
        public const int SW_HIDE = 0;
        public const int SW_SHOW = 5;

        [DllImport("kernel32.dll")]
        public static extern IntPtr GetConsoleWindow();

        [DllImport("user32.dll")]
        public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        // x1nix: added
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern uint GetFinalPathNameByHandleW(IntPtr hFile, IntPtr lpszFilePath, uint cchFilePath = 0u, uint dwFlags = 0u);
    }
}
