// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    public class DebuggerSymbolResult
    {
        public bool Success { get; set; } = false;
    }

    public abstract class DebuggerSymbolProvider
    {
        public virtual DebuggerSymbolResult ResolveAddress(uint Address) { return new DebuggerSymbolResult(); }
    }
}
