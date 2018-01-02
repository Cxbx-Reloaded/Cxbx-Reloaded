// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    public abstract class DebuggerEventListener
    {
        public DebuggerEventListener()
        {
        }
        
        public virtual void OnDebugStart() { }
        public virtual void OnDebugEnd() { }

        public virtual void OnProcessCreate() { }
        public virtual void OnProcessExit() { }

        public virtual void OnThreadCreate() { }
        public virtual void OnThreadExit() { }

        public virtual void OnModuleLoaded(string ModuleName) { }
        public virtual void OnModuleUnloaded() { }

        public virtual void OnCallstack(string[] Callstack) { }

        public virtual void OnDebugOutput(string Message) { }
    }
}
