// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Text;
using WinProcesses = VsChromium.Core.Win32.Processes;
using WinDebug = VsChromium.Core.Win32.Debugging;
using System.Runtime.InteropServices;

namespace CxbxDebugger
{
    public class Debugger : IDebuggerOutputEvents, IDisposable
    {
        enum RunState
        {
            NotLaunched,
            Running,
            Ended
        };

        // Cached process information
        WinProcesses.PROCESS_INFORMATION stProcessInfo = new WinProcesses.PROCESS_INFORMATION();
        WinProcesses.STARTUPINFO stStartInfo = new WinProcesses.STARTUPINFO();
        
        // Wrapped so these can be closed automatically
        WinProcesses.SafeProcessHandle hProcess = new WinProcesses.SafeProcessHandle();
        WinProcesses.SafeThreadHandle hThread = new WinProcesses.SafeThreadHandle();

        string[] args = new string[] { };

        RunState State = RunState.NotLaunched;
        
        DebuggerInstance DebugInstance;

        List<IDebuggerGeneralEvents> GeneralEvents = new List<IDebuggerGeneralEvents>();
        List<IDebuggerProcessEvents> ProcessEvents = new List<IDebuggerProcessEvents>();
        List<IDebuggerThreadEvents> ThreadEvents = new List<IDebuggerThreadEvents>();
        List<IDebuggerModuleEvents> ModuleEvents = new List<IDebuggerModuleEvents>();
        List<IDebuggerOutputEvents> OutputEvents = new List<IDebuggerOutputEvents>();
        List<IDebuggerExceptionEvents> ExceptionEvents = new List<IDebuggerExceptionEvents>();

        DebuggerSymbolServer SymbolSrv;
        KernelProvider KernelSymbolProvider;

        private void Init()
        {
            DebugInstance = null;

            SymbolSrv = new DebuggerSymbolServer();

            KernelSymbolProvider = new KernelProvider();
            SymbolSrv.RegisterProvider(KernelSymbolProvider);

            RegisterEventInterfaces(this);
        }

        public Debugger()
        {
            Init();
        }

        public Debugger(string[] x_args)
        {
            Init();

            if (x_args == null)
                return;

            // Copy all arguments
            args = x_args;

            // Keep quotes for any potential file paths
            for(int i = 0; i < args.Length; ++i)
            {
                if( args[i].IndexOf(":\\") == 1)
                {
                    args[i] = string.Format("\"{0}\"", args[i]);
                }
            }
        }

        public void Dispose()
        {
            // Destroy process
            WinProcesses.NativeMethods.TerminateProcess(hProcess, 0);

            // close it
            hProcess.Close();
            hThread.Close();
        }

        public void Break()
        {
            if (DebugInstance != null)
            {
                DebugInstance.MainProcess.Suspend();
            }
        }

        public void Resume()
        {
            if (DebugInstance != null)
            {
                DebugInstance.MainProcess.Resume();
            }
        }

        public bool CanLaunch()
        {
            switch (State)
            {
                case RunState.NotLaunched:
                case RunState.Ended:
                    return true;
            }

            return false;
        }
        
        static string CxbxDebuggerPrefix = "CxbxDebugger! ";
        static string KernelImportPrefix = "KernelImport_";
        public void OnDebugOutput(string Message)
        {
            if (Message.StartsWith(CxbxDebuggerPrefix))
            {
                string Payload = Message.Substring(CxbxDebuggerPrefix.Length);

                if (Payload.StartsWith(KernelImportPrefix))
                {
                    KernelSymbolProvider.AddKernelSymbolFromMessage(Payload);
                }
                else
                {
                    // TODO Ensure this 
                    SetupHLECacheProvider(Payload);
                }
            }
        }

        private void SetupHLECacheProvider(string Filename)
        {
            var Provider = new HLECacheProvider();
            if( Provider.Load(Filename) )
            {
                SymbolSrv.RegisterProvider(Provider);
            }
        }

        public bool Launch()
        {
            if (CanLaunch() == false)
                throw new Exception("Unable to launch in this state");

            var DebugCreationFlags =
                WinProcesses.ProcessCreationFlags.DEBUG_ONLY_THIS_PROCESS |
                WinProcesses.ProcessCreationFlags.CREATE_NEW_CONSOLE;
            
            bool bRet = WinProcesses.NativeMethods.CreateProcess
            (
                null,
                new StringBuilder(string.Join(" ", args)),
                null,
                null,
                false,
                DebugCreationFlags,
                null,
                null,
                stStartInfo,
                stProcessInfo
            );

            if (bRet == true)
            {
                // Store these so they can be marshalled, and closed correctly
                // TODO Move to Debugger* classes
                hProcess = new WinProcesses.SafeProcessHandle(stProcessInfo.hProcess);
                hThread = new WinProcesses.SafeThreadHandle(stProcessInfo.hThread);

                var Process = new DebuggerProcess();
                
                Process.Handle = stProcessInfo.hProcess;
                Process.ProcessID = (uint)stProcessInfo.dwProcessId;

                var Thread = new DebuggerThread(Process);

                Thread.Handle = stProcessInfo.hThread;
                Thread.ThreadID = NativeMethods.GetThreadId(Thread.Handle);

                // TODO: Fill out other Thread properties

                Process.Threads.Add(Thread);
                Process.MainThread = Thread;

                DebugInstance = new DebuggerInstance(Process);

                RegisterEventInterfaces(DebugInstance);

                State = RunState.Running;
            }

            return bRet;
        }

        private string ResolveProcessPath(IntPtr FileHandle)
        {
            string path = "";

            const int bufSize = 1024;
            var strPtr = Marshal.AllocHGlobal(bufSize);

            uint length = LowLevelDesign.Win32.Windows.NativeMethods.GetFinalPathNameByHandleW
            (
                FileHandle,
                strPtr,
                bufSize,
                0
            );

            if (length != 0)
            {
                path = Marshal.PtrToStringUni(strPtr, (int)length);
                path = path.TrimStart('\\', '?');
            }
            Marshal.FreeHGlobal(strPtr);

            return path;
        }

        private string ReadProcessString(IntPtr NamePtr, uint Length, bool Unicode)
        {
            string resultStr = "";

            if (Length == 0 || Length > 512)
                return resultStr;

            byte[] buffer = new byte[Length];
            
            uint numRead = 0;
            WinProcesses.NativeMethods.ReadProcessMemory
            (
                hProcess,
                NamePtr,
                buffer,
                Length,
                out numRead
            );

            if( numRead == Length )
            {
                // FIXME Questionable Unicode support here
                if (Unicode)
                {
                    resultStr = Encoding.Unicode.GetString(buffer, 0, (int)numRead);
                }
                else
                {
                    // TODO Investigate why the read length includes the nul-terminator
                    if (numRead > 1) --numRead;

                    resultStr = Encoding.ASCII.GetString(buffer, 0, (int)numRead);
                }
            }

            return resultStr;
        }

        private uint GetMemory(IntPtr addr)
        {
            // TODO: Template this for different sizes

            byte[] buffer = new byte[4];
            
            uint numRead = 0;
            WinProcesses.NativeMethods.ReadProcessMemory
            (
                hProcess,
                addr,
                buffer,
                4,
                out numRead
            );

            return BitConverter.ToUInt32(buffer, 0);
        }

        private void HandleCreateThread(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.CreateThread;

            var Thread = new DebuggerThread(DebugInstance.MainProcess);

            Thread.Handle = DebugInfo.hThread;
            Thread.ThreadID = Thread.ThreadID = NativeMethods.GetThreadId(Thread.Handle);
            Thread.StartAddress = DebugInfo.lpStartAddress;
            Thread.ThreadBase = DebugInfo.lpThreadLocalBase;

            foreach (IDebuggerThreadEvents Event in ThreadEvents )
            {
                Event.OnThreadCreate(Thread);
            }
        }
        
        private void HandleExitThread(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.ExitThread;

            var TargetThread = DebugInstance.MainProcess.Threads.Find(Thread => Thread.ThreadID == DebugEvent.dwThreadId);
            uint ExitCode = DebugInfo.dwExitCode;

            if (TargetThread != null)
            {
                foreach (IDebuggerThreadEvents Event in ThreadEvents)
                {
                    Event.OnThreadExit(TargetThread, ExitCode);
                }
            }
        }

        private void HandleCreateProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.CreateProcessInfo;

            foreach (IDebuggerProcessEvents Event in ProcessEvents)
            {
                Event.OnProcessCreate(null);
            }
        }

        private void HandleExitProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.ExitProcess;

            var TargetProcess = DebugInstance.Processes.Find(Process => Process.ProcessID == DebugEvent.dwProcessId);
            uint ExitCode = DebugInfo.dwExitCode;

            if (TargetProcess != null)
            {
                foreach (IDebuggerProcessEvents Event in ProcessEvents)
                {
                    Event.OnProcessExit(TargetProcess, ExitCode);
                }
            }
        }

        private void HandleLoadDll(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.LoadDll;

            var Module = new DebuggerModule();
            
            Module.Path = ResolveProcessPath(DebugInfo.hFile);
            Module.ImageBase = DebugInfo.lpBaseOfDll;

            foreach (IDebuggerModuleEvents Event in ModuleEvents)
            {
                Event.OnModuleLoaded(Module);
            }
        }

        private void HandleUnloadDll(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.UnloadDll;

            var TargetModule = DebugInstance.MainProcess.Modules.Find(Module => Module.ImageBase == DebugInfo.lpBaseOfDll);

            if (TargetModule != null)
            {
                foreach (IDebuggerModuleEvents Event in ModuleEvents)
                {
                    Event.OnModuleUnloaded(TargetModule);
                }
            }
        }

        private void HandleOutputDebugString(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.DebugString;

            string debugString = ReadProcessString(DebugInfo.lpDebugStringData, DebugInfo.nDebugStringLength, DebugInfo.fUnicode == 1);

            foreach(IDebuggerOutputEvents Event in OutputEvents)
            {
                Event.OnDebugOutput(debugString);
            }
        }

        private void HandleException(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.Exception;

            switch ((ExceptionCode)DebugInfo.ExceptionRecord.ExceptionCode)
            {
                case ExceptionCode.AccessViolation:
                    
                    // Pauses execution
                    Break();

                    foreach(IDebuggerExceptionEvents Event in ExceptionEvents)
                    {
                        Event.OnAccessViolation();
                    }

                    break;

                case ExceptionCode.Breakpoint:
                    // TODO Handle
                    break;

                case ExceptionCode.SingleStep:
                    // TODO Handle
                    break;
            }
            
            // hmm!!
        }

        public void Run()
        {
            WinDebug.DEBUG_EVENT DbgEvt = new WinDebug.DEBUG_EVENT();
            
            WinDebug.CONTINUE_STATUS ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
            bool bContinue = true;

            foreach (IDebuggerGeneralEvents Event in GeneralEvents)
            {
                Event.OnDebugStart();
            }

            // Loop until told to stop
            while (bContinue == true)
            {
                // Pause until a debug event notification happens
                bContinue = WinDebug.NativeMethods.WaitForDebugEvent(out DbgEvt, VsChromium.Core.Win32.Constants.INFINITE);

                switch (DbgEvt.dwDebugEventCode)
                {
                    case WinDebug.DEBUG_EVENT_CODE.EXCEPTION_DEBUG_EVENT:
                        HandleException(DbgEvt);
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.CREATE_THREAD_DEBUG_EVENT:
                        HandleCreateThread(DbgEvt);
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.CREATE_PROCESS_DEBUG_EVENT:
                        // TODO: Limit support for multiple processes
                        HandleCreateProcess(DbgEvt);
                        break;
                        
                    case WinDebug.DEBUG_EVENT_CODE.EXIT_THREAD_DEBUG_EVENT:
                        HandleExitThread(DbgEvt);
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.EXIT_PROCESS_DEBUG_EVENT:
                        HandleExitProcess(DbgEvt);

                        // XX Temporary
                        bContinue = false;
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.LOAD_DLL_DEBUG_EVENT:
                        HandleLoadDll(DbgEvt);
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.UNLOAD_DLL_DEBUG_EVENT:
                        HandleUnloadDll(DbgEvt);
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.OUTPUT_DEBUG_STRING_EVENT:
                        HandleOutputDebugString(DbgEvt);
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.RIP_EVENT:
                        // TODO: Handle
                        break;
                }

                WinDebug.NativeMethods.ContinueDebugEvent(DbgEvt.dwProcessId, DbgEvt.dwThreadId, ContinueStatus);
            }
            
            State = RunState.Ended;

            foreach (IDebuggerGeneralEvents Event in GeneralEvents)
            {
                Event.OnDebugEnd();
            }
        }

        public DebuggerSymbol ResolveSymbol(uint Address)
        {
            return SymbolSrv.FindSymbol(Address);
        }

        public DebuggerSymbol ResolveSymbol(IntPtr Address)
        {
            return SymbolSrv.FindSymbol((uint)Address);
        }

        public void RegisterEventInterfaces(object EventClass)
        {
            IDebuggerGeneralEvents GeneralListener = EventClass as IDebuggerGeneralEvents;
            if(GeneralListener != null ) GeneralEvents.Add(GeneralListener);

            IDebuggerProcessEvents ProcessListener = EventClass as IDebuggerProcessEvents;
            if (ProcessListener != null) ProcessEvents.Add(ProcessListener);

            IDebuggerThreadEvents ThreadListener = EventClass as IDebuggerThreadEvents;
            if (ThreadListener != null) ThreadEvents.Add(ThreadListener);

            IDebuggerModuleEvents ModuleListener = EventClass as IDebuggerModuleEvents;
            if (ModuleListener != null) ModuleEvents.Add(ModuleListener);

            IDebuggerOutputEvents OutputListener = EventClass as IDebuggerOutputEvents;
            if (OutputListener != null) OutputEvents.Add(OutputListener);

            IDebuggerExceptionEvents ExceptionListener = EventClass as IDebuggerExceptionEvents;
            if (ExceptionListener != null) ExceptionEvents.Add(ExceptionListener);
        }
    }
}
