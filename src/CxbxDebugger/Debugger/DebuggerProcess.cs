// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using WinProcesses = VsChromium.Core.Win32.Processes;

namespace CxbxDebugger
{
    public class DebuggerProcess
    {
        public IntPtr Handle { get; set; }
        public uint ProcessID { get; set; }

        public List<DebuggerModule> Modules { get; set; }
        public List<DebuggerThread> Threads { get; set; }

        public DebuggerModule MainModule { get; set; }
        public DebuggerThread MainThread { get; set; }

        // Based on DebugProcess
        public DebuggerProcess()
        {
            Handle = IntPtr.Zero;
            ProcessID = 0;

            Modules = new List<DebuggerModule>();
            Threads = new List<DebuggerThread>();

            MainModule = null;
            MainThread = null;
        }

        public void Suspend()
        {
            // TODO Thread may already be in a non-suspended state

            foreach (DebuggerThread Thread in Threads)
            {
                WinProcesses.NativeMethods.SuspendThread(Thread.Handle);
            }
        }

        public void Resume()
        {
            // TODO Thread may already be in a non-suspended state

            foreach (DebuggerThread Thread in Threads)
            {
                WinProcesses.NativeMethods.ResumeThread(Thread.Handle);
            }
        }

        public uint ReadMemory(IntPtr addr)
        {
            byte[] buffer = new byte[4];

            uint numRead = 0;
            WinProcesses.NativeMethods.ReadProcessMemory
            (
                Handle,
                addr,
                buffer,
                4,
                out numRead
            );

            return BitConverter.ToUInt32(buffer, 0);
        }

    }
}
