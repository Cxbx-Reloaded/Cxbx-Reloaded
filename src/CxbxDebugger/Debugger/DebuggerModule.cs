// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;

namespace CxbxDebugger
{
    public class DebuggerModule
    {
        public IntPtr ImageBase { get; set; }
        public string Path { get; set; }

        // Based on DebugProcessModule
        public DebuggerModule()
        {
            ImageBase = IntPtr.Zero;
            Path = "";
        }
    }
}
