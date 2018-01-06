// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;

namespace CxbxDebugger
{
    public class DebuggerInstance : IDebuggerModuleEvents, IDebuggerThreadEvents
    {
        public List<DebuggerProcess> Processes { get; set; }

        public DebuggerProcess MainProcess { get; set; }
        public DebuggerThread CurrentThread { get; set; }

        public DebuggerInstance(DebuggerProcess InitialProcess)
        {
            Processes = new List<DebuggerProcess>();
            Processes.Add(InitialProcess);

            MainProcess = InitialProcess;
            CurrentThread = InitialProcess.MainThread;
        }

        public void OnThreadCreate(DebuggerThread Thread)
        {
            MainProcess.Threads.Add(Thread);
        }

        public void OnThreadExit(DebuggerThread Thread, uint ExitCode)
        {
            MainProcess.Threads.Remove(Thread);
        }

        public void OnModuleLoaded(DebuggerModule Module)
        {
            MainProcess.Modules.Add(Module);
        }

        public void OnModuleUnloaded(DebuggerModule Module)
        {
            MainProcess.Modules.Remove(Module);
        }
    }
}
