// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    public abstract class DebuggerEventListener
    {
        public virtual void OnDebugStart() { }
        public virtual void OnDebugEnd() { }

        public virtual void OnProcessCreate(DebuggerProcess Process) { }
        public virtual void OnProcessExit(DebuggerProcess Process) { }

        public virtual void OnThreadCreate(DebuggerThread Thread) { }
        public virtual void OnThreadExit(DebuggerThread Thread) { }

        public virtual void OnModuleLoaded(DebuggerModule Module) { }
        public virtual void OnModuleUnloaded(DebuggerModule Module) { }

        public virtual void OnCallstack(DebuggerCallstack Callstack) { }

        public virtual void OnDebugOutput(string Message) { }
    }
}
