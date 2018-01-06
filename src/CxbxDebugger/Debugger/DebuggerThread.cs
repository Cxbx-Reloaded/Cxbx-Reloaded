// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;

namespace CxbxDebugger
{
    public class DebuggerThread
    {
        public DebuggerProcess OwningProcess { get; set; }

        public IntPtr Handle { get; set; }
        public uint ThreadID { get; set; }
        public IntPtr StartAddress { get; set; }
        public IntPtr ThreadBase { get; set; }

        CONTEXT_x86 ContextCache = new CONTEXT_x86
        {
            ContextFlags =
                            ContextFlags.CONTEXT_i386 |
                            ContextFlags.CONTEXT_CONTROL |
                            ContextFlags.CONTEXT_INTEGER |
                            ContextFlags.CONTEXT_SEGMENTS
        };

        DebuggerCallstack CallstackCache = new DebuggerCallstack();

        // Based on DebugThread
        public DebuggerThread(DebuggerProcess Owner)
        {
            OwningProcess = Owner;

            Handle = IntPtr.Zero;
            ThreadID = 0;
            StartAddress = IntPtr.Zero;
            ThreadBase = IntPtr.Zero;
        }

        public void UpdateContext()
        {
            bool Result = NativeMethods.GetThreadContext(Handle, ref ContextCache);
            if (!Result)
                return;
            
            var ebp = ContextCache.ebp;

            CallstackCache = new DebuggerCallstack();

            CallstackCache.AddFrame(new DebuggerStackFrame(new IntPtr(ContextCache.eip), new IntPtr(ebp), new IntPtr(ContextCache.esp)));

            do
            {
                var ReturnAddr = OwningProcess.ReadMemory<uint>(new IntPtr(ebp + 4));
                ebp = OwningProcess.ReadMemory<uint>(new IntPtr(ebp));

                if (ebp == 0 || ReturnAddr == ebp)
                    break;

                CallstackCache.AddFrame(new DebuggerStackFrame(new IntPtr(ReturnAddr), new IntPtr(ebp)));
            }
            while (!CallstackCache.HasEnoughFrames);
        }
    }
}
