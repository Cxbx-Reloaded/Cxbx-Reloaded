// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;

namespace CxbxDebugger
{
    public class DebuggerStackFrame
    {
        public IntPtr PC { get; }
        public IntPtr Base { get; }
        public IntPtr Stack { get; }

        // TOOD Resolve symbol for this frame

        public DebuggerStackFrame(IntPtr EIP, IntPtr EBP, IntPtr ESP)
        {
            PC = EIP;
            Base = EBP;
            Stack = ESP;
        }

        public DebuggerStackFrame(IntPtr EIP, IntPtr EBP)
        {
            PC = EIP;
            Base = EBP;
            Stack = IntPtr.Zero;
        }
    }

    public class DebuggerCallstack
    {
        int NumSupportedFrames = 16;
        public List<DebuggerStackFrame> StackFrames { get; }

        public DebuggerCallstack()
        {
            StackFrames = new List<DebuggerStackFrame>(NumSupportedFrames);
        }

        public DebuggerCallstack(int MaxFrames)
        {
            NumSupportedFrames = MaxFrames;
            StackFrames = new List<DebuggerStackFrame>(NumSupportedFrames);
        }

        public void AddFrame(DebuggerStackFrame StackFrame)
        {
            if (CanCollect)
            {
                StackFrames.Add(StackFrame);
            }
        }

        public bool CanCollect
        {
            get
            {
                return StackFrames.Count < NumSupportedFrames;
            }
        }
    }
}
