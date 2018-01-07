// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;

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
        void OnAccessViolation(DebuggerThread Thread, IntPtr Address);
    }

    public interface IDebuggerFileEvents
    {
        void OnFileOpened(IntPtr Handle, string Name);
        void OnFileRead(IntPtr Handle, uint Length);
        void OnFileClosed(IntPtr Handle);
    }
}
