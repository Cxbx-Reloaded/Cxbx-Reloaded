// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    public interface IDebuggerGeneralEvents
    {
        void OnDebugStart();
        void OnDebugEnd();
    }

    public interface IDebuggerProcessEvents
    {
        void OnProcessCreate(DebuggerProcess Process);
        void OnProcessExit(DebuggerProcess Process, uint ExitCode);
    }

    public interface IDebuggerThreadEvents
    {
        void OnThreadCreate(DebuggerThread Thread);
        void OnThreadExit(DebuggerThread Thread, uint ExitCode);
    }

    public interface IDebuggerModuleEvents
    {
        void OnModuleLoaded(DebuggerModule Module);
        void OnModuleUnloaded(DebuggerModule Module);
    }

    public interface IDebuggerOutputEvents
    {
        void OnDebugOutput(string Message);
    }

    public interface IDebuggerExceptionEvents
    {
        void OnAccessViolation();
    }
}
