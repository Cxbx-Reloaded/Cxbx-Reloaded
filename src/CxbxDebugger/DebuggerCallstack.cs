using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CxbxDebugger
{
    public class DebuggerStackFrame
    {
        public IntPtr BasePointer { get; }
        public IntPtr CodeAddress { get; }

        public DebuggerStackFrame(IntPtr Base, IntPtr CodeAddr)
        {
            BasePointer = Base;
            CodeAddress = CodeAddr;
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
            if (!HasEnoughFrames)
            {
                StackFrames.Add(StackFrame);
            }
        }

        public bool HasEnoughFrames
        {
            get
            {
                return StackFrames.Count < NumSupportedFrames;
            }
        }
    }
}
