// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;

namespace CxbxDebugger
{
    public class DebuggerInstance
        : IDebuggerProcessEvents
        , IDebuggerModuleEvents
        , IDebuggerThreadEvents
    {
        public List<DebuggerProcess> Processes { get; set; }

        public DebuggerProcess MainProcess { get; set; }
        public DebuggerThread CurrentThread { get; set; }

        private List<uint> AddressLookup = new List<uint>();

        public DebuggerInstance(DebuggerProcess InitialProcess)
        {
            Processes = new List<DebuggerProcess>();
            Processes.Add(InitialProcess);

            MainProcess = InitialProcess;
            CurrentThread = InitialProcess.MainThread;
        }

        public DebuggerModule FindModuleFromAddress(uint Address)
        {
            int Result = AddressLookup.FindLastIndex(Addr => Addr <= Address);

            if (Result == -1)
                return null;

            // The MainProcess is also treated as a module
            if( (uint)MainProcess.ImageBase == AddressLookup[Result] )
                return MainProcess;

            return MainProcess.Modules.Find(Module => (uint)Module.ImageBase == AddressLookup[Result]);
        }

        public DebuggerProcess FindProcess(uint ProcessID)
        {
            return Processes.Find(Process => Process.ProcessID == ProcessID);
        }

        public void OnProcessCreate(DebuggerProcess Process)
        {
            AddressLookup.Add((uint)Process.ImageBase);
            AddressLookup.Sort();
        }

        public void OnProcessExit(DebuggerProcess Process, uint ExitCode)
        {
            AddressLookup.Remove((uint)Process.ImageBase);
        }

        public void OnThreadCreate(DebuggerThread Thread)
        {
            MainProcess.Threads.Add(Thread);
        }

        public void OnThreadExit(DebuggerThread Thread, uint ExitCode)
        {
            MainProcess.Threads.Remove(Thread);
        }

        public void OnThreadNamed(DebuggerThread Thread)
        {
            // Unused
        }

        public void OnModuleLoaded(DebuggerModule Module)
        {
            MainProcess.Modules.Add(Module);

            AddressLookup.Add((uint)Module.ImageBase);
            AddressLookup.Sort();
        }

        public void OnModuleUnloaded(DebuggerModule Module)
        {
            MainProcess.Modules.Remove(Module);

            AddressLookup.Remove((uint)Module.ImageBase);
        }
    }
}
