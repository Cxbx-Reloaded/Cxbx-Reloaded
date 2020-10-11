// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;

namespace CxbxDebugger
{
    public interface IDebuggerSessionEvents
    {
        void OnDebugStart();
        void OnDebugEnd();
        void OnDebugTitleLoaded(string Title);
        void OnDebugTargetChanged(string CommandLine);
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
        void OnThreadNamed(DebuggerThread Thread);
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
        bool OnAccessViolation(DebuggerThread Thread, uint Code, uint Address);
        void OnBreakpoint(DebuggerThread Thread, uint Address, uint Code, bool FirstChance);
    }

    public interface IDebuggerFileEvents
    {
        void OnFileOpened(DebuggerMessages.FileOpened Info);
        void OnFileRead(DebuggerMessages.FileRead Info);
        void OnFileWrite(DebuggerMessages.FileWrite Info);
        void OnFileClosed(DebuggerMessages.FileClosed Info);
    }
}
