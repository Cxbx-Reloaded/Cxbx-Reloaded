// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using WinProcesses = VsChromium.Core.Win32.Processes;

namespace CxbxDebugger
{
    public class DebuggerThread
    {
        public DebuggerProcess OwningProcess { get; set; }

        public IntPtr Handle { get; set; }
        public uint ThreadID { get; set; }
        public IntPtr StartAddress { get; set; }
        public IntPtr ThreadBase { get; set; }
        public string DebugName { get; set; }

        private int CachedSuspendCount = 0;

        public bool IsSuspended
        {
            get
            {
                // TODO: Check count is inside range [0..MAXIMUM_SUSPEND_COUNT]
                return CachedSuspendCount > 0;
            }
        }

        public bool WasSuspended
        {
            get
            {
                return CachedSuspendCount > 1;
            }
        }

        CONTEXT_x86 ContextCache = new CONTEXT_x86
        {
            ContextFlags =
                            ContextFlags.CONTEXT_i386 |
                            ContextFlags.CONTEXT_CONTROL |
                            ContextFlags.CONTEXT_INTEGER |
                            ContextFlags.CONTEXT_SEGMENTS
        };

        public DebuggerCallstack CallstackCache { get; private set; } = new DebuggerCallstack();

        // Based on DebugThread
        public DebuggerThread(DebuggerProcess Owner)
        {
            OwningProcess = Owner;

            Handle = IntPtr.Zero;
            ThreadID = 0;
            StartAddress = IntPtr.Zero;
            ThreadBase = IntPtr.Zero;
        }

        public void Suspend()
        {
            // Early-out if this thread was already in a suspended state
            if (IsSuspended)
                return;

            int PrevSuspendCount = (int)WinProcesses.NativeMethods.SuspendThread(Handle);
            if (PrevSuspendCount == -1)
                throw new Exception("SuspendThread failed");

            CachedSuspendCount = PrevSuspendCount +1;
        }

        public void Resume(bool ForceStart = false)
        {
            // Early-out if this thread was not already suspended
            if (!IsSuspended)
                return;

            // Stores the internal "suspend counter" back to the previous state - which could still be suspended

            int TargetCount = (ForceStart ? 0 : CachedSuspendCount - 1);
            int LastSuspendCount;

            do
            {
                LastSuspendCount = (int)WinProcesses.NativeMethods.ResumeThread(Handle);
                if (LastSuspendCount == -1)
                    throw new Exception("ResumeThread failed");
            }
            while (LastSuspendCount -1 > TargetCount);

            CachedSuspendCount = TargetCount;
        }

        public void UpdateContext()
        {
            bool Result = NativeMethods.GetThreadContext(Handle, ref ContextCache);
            if (!Result)
                return;
            
            uint ebp = ContextCache.ebp;

            CallstackCache = new DebuggerCallstack();
            CallstackCache.AddFrame(new DebuggerStackFrame(new IntPtr(ContextCache.eip), new IntPtr(ebp), new IntPtr(ContextCache.esp)));

            uint ReturnAddr = 0;
            do
            {
                if (!OwningProcess.ReadMemory(new IntPtr(ebp + 4), ref ReturnAddr))
                    break;
                if (!OwningProcess.ReadMemory(new IntPtr(ebp), ref ebp))
                    break;

                if (ebp == 0 || ReturnAddr == ebp)
                    break;

                if ((ReturnAddr & 0x80000000) != 0)
                    break;

                if ((ebp & 0x80000000) != 0)
                    break;

                CallstackCache.AddFrame(new DebuggerStackFrame(new IntPtr(ReturnAddr), new IntPtr(ebp)));
            }
            while (CallstackCache.CanCollect);
        }
    }
}
