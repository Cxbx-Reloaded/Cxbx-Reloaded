// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Text;
using WinProcesses = VsChromium.Core.Win32.Processes;
using WinDebug = VsChromium.Core.Win32.Debugging;
using System.Runtime.InteropServices;
using WinLowLevel = LowLevelDesign.Win32.Windows.NativeMethods;
using System.Threading;
using System.Threading.Tasks;

namespace CxbxDebugger
{
    public class Debugger : IDisposable
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
        string Target = "";

        public string ProcessName
        {
            get { return Target; }
        }

        RunState State = RunState.NotLaunched;

        DebuggerMessages.DebuggerInit InitParams = new DebuggerMessages.DebuggerInit();
        
        DebuggerInstance DebugInstance;

        List<IDebuggerSessionEvents> SessionEvents = new List<IDebuggerSessionEvents>();
        List<IDebuggerProcessEvents> ProcessEvents = new List<IDebuggerProcessEvents>();
        List<IDebuggerThreadEvents> ThreadEvents = new List<IDebuggerThreadEvents>();
        List<IDebuggerModuleEvents> ModuleEvents = new List<IDebuggerModuleEvents>();
        List<IDebuggerOutputEvents> OutputEvents = new List<IDebuggerOutputEvents>();
        List<IDebuggerExceptionEvents> ExceptionEvents = new List<IDebuggerExceptionEvents>();
        List<IDebuggerFileEvents> FileEvents = new List<IDebuggerFileEvents>();

        DebuggerSymbolServer SymbolSrv;
        KernelProvider KernelSymbolProvider;

        ManualResetEvent bpStall = new ManualResetEvent(false);

        bool bContinue = true;
        WinDebug.CONTINUE_STATUS ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;

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

        public Debugger(string[] launchArgs)
        {
            Init();

            // Copy XBE path without the quotes
            var loadArg = Array.FindIndex(launchArgs, x => x == "/load");
            if (loadArg != -1)
            {
                Target = launchArgs[loadArg + 1].Trim(new char[] { '"' });
            }

            args = new string[launchArgs.Length];

            for (int i = 0; i < launchArgs.Length; ++i)
            {
                var arg = launchArgs[i];

                if (arg.Contains(" "))
                {
                    arg = string.Format($"\"{arg}\"");
                }

                args[i] = arg;
            }
        }

        public void Dispose()
        {
            bContinue = false;
            bpStall.Set();

            // Remove all events
            SessionEvents.Clear();
            ProcessEvents.Clear();
            ThreadEvents.Clear();
            ModuleEvents.Clear();
            OutputEvents.Clear();
            ExceptionEvents.Clear();
            FileEvents.Clear();

            // Destroy process
            if (!hProcess.IsClosed)
            {
                WinProcesses.NativeMethods.TerminateProcess(hProcess, 0);
            }
            
            // Close handles
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

        public void Trace()
        {
            if (DebugInstance != null)
            {
                DebugInstance.MainProcess.Trace();
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

            if (args.Length == 0)
                return false;

            var DebugCreationFlags =
                WinProcesses.ProcessCreationFlags.DEBUG_ONLY_THIS_PROCESS |
                WinProcesses.ProcessCreationFlags.CREATE_NEW_CONSOLE;

            var launchArgs = new StringBuilder(string.Join(" ", args));

            bool bRet = WinProcesses.NativeMethods.CreateProcess
            (
                null,
                launchArgs,
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
                // Store so they can be marshalled and closed correctly
                hProcess = new WinProcesses.SafeProcessHandle(stProcessInfo.hProcess);
                hThread = new WinProcesses.SafeThreadHandle(stProcessInfo.hThread);

                bContinue = true;

                State = RunState.Running;
            }
            else
            {
                throw new Exception($"Failed to launch loader '{launchArgs}'");
            }

            return bRet;
        }

        private string ResolveProcessPath(IntPtr FileHandle)
        {
            string path = "";

            const int bufSize = 1024;
            var strPtr = Marshal.AllocHGlobal(bufSize);

            uint length = WinLowLevel.GetFinalPathNameByHandleW
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

            if ((uint)DebugEvent.dwProcessId != DebugInstance.MainProcess.ProcessID)
            {
                throw new Exception("Handling event for unknown process");
            }
            
            var Process = DebugInstance.FindProcess((uint)DebugEvent.dwProcessId);

            if (Process == null)
                throw new Exception("Unable to create tread with untracked process id");

            var Thread = new DebuggerThread(Process);

            Thread.Handle = DebugInfo.hThread;
            Thread.ThreadID = NativeMethods.GetThreadId(Thread.Handle);
            Thread.StartAddress = DebugInfo.lpStartAddress;
            Thread.ThreadBase = DebugInfo.lpThreadLocalBase;

            Parallel.ForEach(ThreadEvents, Event => Event.OnThreadCreate(Thread));
        }
        
        private void HandleExitThread(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.ExitThread;

            if ((uint)DebugEvent.dwProcessId != DebugInstance.MainProcess.ProcessID)
            {
                throw new Exception("Handling event for unknown process");
            }

            var TargetThread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
            if (TargetThread != null)
            {
                uint ExitCode = DebugInfo.dwExitCode;

                Parallel.ForEach(ThreadEvents, Event => Event.OnThreadExit(TargetThread, ExitCode));
            }
        }

        const int VM_PLACEHOLDER_SIZE = 0x3FEF000;
        
        private void HandleCreateProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.CreateProcessInfo;

            var Process = new DebuggerProcess();

            Process.Core = true;
            Process.Handle = DebugInfo.hProcess;
            Process.ProcessID = WinLowLevel.GetProcessId(Process.Handle);
            Process.Path = ResolveProcessPath(DebugInfo.hFile);

            // Skip over allocated Xbox memory
            Process.ImageBase = DebugInfo.lpBaseOfImage + VM_PLACEHOLDER_SIZE; 

            var MainThread = new DebuggerThread(Process);

            MainThread.Handle = DebugInfo.hThread;
            MainThread.ThreadID = NativeMethods.GetThreadId(DebugInfo.hThread);
            MainThread.ThreadBase = DebugInfo.lpThreadLocalBase;

            // Setup as the main thread
            // TODO Check that we need to treat this as a special case
            Process.MainThread = MainThread;

            DebugInstance = new DebuggerInstance(Process);
            RegisterEventInterfaces(DebugInstance);

            Parallel.ForEach(ProcessEvents, Event => Event.OnProcessCreate(Process));

            Parallel.ForEach(ThreadEvents, Event => Event.OnThreadCreate(MainThread));
                        
            var XboxModule = new DebuggerModule();
            
            XboxModule.Path = Target;
            XboxModule.ImageBase = DebugInfo.lpBaseOfImage;
            XboxModule.Core = true;

            Parallel.ForEach(ModuleEvents, Event => Event.OnModuleLoaded(XboxModule));
        }

        private void HandleExitProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.ExitProcess;

            if ((uint)DebugEvent.dwProcessId != DebugInstance.MainProcess.ProcessID)
            {
                throw new Exception("Handling event for unknown process");
            }

            var TargetProcess = DebugInstance.FindProcess((uint)DebugEvent.dwProcessId);
            uint ExitCode = DebugInfo.dwExitCode;

            if (TargetProcess != null)
            {
                Parallel.ForEach(ProcessEvents, Event => Event.OnProcessExit(TargetProcess, ExitCode));
            }
        }

        private void HandleLoadDll(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.LoadDll;

            if ((uint)DebugEvent.dwProcessId != DebugInstance.MainProcess.ProcessID)
            {
                throw new Exception("Handling event for unknown process");
            }

            var Module = new DebuggerModule();
            
            Module.Path = ResolveProcessPath(DebugInfo.hFile);
            Module.ImageBase = DebugInfo.lpBaseOfDll;

            Parallel.ForEach(ModuleEvents, Event => Event.OnModuleLoaded(Module));
        }

        private void HandleUnloadDll(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.UnloadDll;

            if ((uint)DebugEvent.dwProcessId != DebugInstance.MainProcess.ProcessID)
            {
                throw new Exception("Handling event for unknown process");
            }

            var TargetModule = DebugInstance.MainProcess.Modules.Find(Module => Module.ImageBase == DebugInfo.lpBaseOfDll);

            if (TargetModule != null)
            {
                Parallel.ForEach(ModuleEvents, Event => Event.OnModuleUnloaded(TargetModule));
            }
        }

        private void HandleOutputDebugString(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.DebugString;

            string debugString = ReadProcessString(DebugInfo.lpDebugStringData, DebugInfo.nDebugStringLength, DebugInfo.fUnicode == 1);

            Parallel.ForEach(OutputEvents, Event => Event.OnDebugOutput(debugString));
        }

        private void HandleException(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.Exception;
            
            switch ((ExceptionCode)DebugInfo.ExceptionRecord.ExceptionCode)
            {
                case ExceptionCode.AccessViolation:
                    {
                        // Ignore all access violation errors.

                        // This will fallback to the debugee (Cxbx) and the exception handling routine.
                        // Should they be unsupported, the debugger "OVERRIDE_EXCEPTION" message is thrown
                        // which means the exception is usually fatal

                        ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_EXCEPTION_NOT_HANDLED;
                    }
                    break;

                case ExceptionCode.PrivilegedInstruction:
                    {
                        // Seeing this frequently called in Win10

                        ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_EXCEPTION_NOT_HANDLED;
                    }
                    break;

                case ExceptionCode.StatusHandleNotClosable:
                    {
                        // Seeing this frequently called in Win10

                        ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_EXCEPTION_NOT_HANDLED;
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.OVERRIDE_EXCEPTION:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Query = DebuggerMessages.GetExceptionHandledQuery(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));
                            
                            bool Handled = false;
                            foreach (IDebuggerExceptionEvents Event in ExceptionEvents)
                            {
                                Handled |= Event.OnAccessViolation(Thread, Query.ExceptionCode, Query.ExceptionAddress);
                            }

                            // Write the reponse to memory
                            Thread.OwningProcess.WriteMemory(Query.ReponseAddr, Handled);
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.HLECACHE_FILE:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetHLECacheReport(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));
                            SetupHLECacheProvider(Report.FileName);
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.KERNEL_PATCH:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetKernelPatchReport(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));

                            KernelSymbolProvider.AddKernelSymbolFromMessage(Report);
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.FILE_OPENED:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetFileOpenedReport(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));

                            Parallel.ForEach(FileEvents, Event => Event.OnFileOpened(Report));
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.FILE_READ:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetFileReadReport(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));

                            Parallel.ForEach(FileEvents, Event => Event.OnFileRead(Report));
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.FILE_WRITE:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetFileWriteReport(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));

                            Parallel.ForEach(FileEvents, Event => Event.OnFileWrite(Report));
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.FILE_CLOSED:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetFileClosedReport(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));
                            if (Report != null)
                            {
                                Parallel.ForEach(FileEvents, Event => Event.OnFileClosed(Report));
                            }
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.DEBUGGER_INIT:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetDebuggerInitReport(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));

                            InitParams = Report;

                            Parallel.ForEach(SessionEvents, Event => Event.OnDebugTitleLoaded(InitParams.Title));
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.DEBUGGER_NEW_TARGET:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetDebuggerNewTargetReport(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));

                            Parallel.ForEach(SessionEvents, Event => Event.OnDebugTargetChanged(Report.CommandLine));
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.MS_VC_EXCEPTION:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetMSVCThreadName(Thread, new DebuggerMessages.DataProcessor(DebugInfo.ExceptionRecord.ExceptionInformation));
                            if(Report != null)
                            {
                                // Resolve the ThreadId of an invalid ID to the current thread name
                                if (Report.ThreadId == uint.MaxValue)
                                {
                                    Report.ThreadId = (uint)DebugEvent.dwThreadId;
                                }

                                var ResolvedThread = DebugInstance.MainProcess.FindThread(Report.ThreadId);
                                if(ResolvedThread != null)
                                {
                                    // Update the resolved thread name
                                    ResolvedThread.DebugName = Report.Name;

                                    Parallel.ForEach(ThreadEvents, Event => Event.OnThreadNamed(Thread));
                                }
                            }
                        }
                    }
                    break;

                case ExceptionCode.Breakpoint:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            uint BpAddr = (uint)DebugInfo.ExceptionRecord.ExceptionAddress;
                            uint BpCode = DebugInfo.ExceptionRecord.ExceptionCode;
                            bool FirstChance = (DebugInfo.dwFirstChance != 0);

                            bpStall.Reset();

                            Parallel.ForEach(ExceptionEvents, Event => Event.OnBreakpoint(Thread, BpAddr, BpCode, FirstChance));

                            bpStall.WaitOne();
                        }
                    }
                    break;

                case ExceptionCode.SingleStep:
                    // TODO Handle
                    break;

                default:
                    ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_EXCEPTION_NOT_HANDLED;
                    break;
            }
        }
        
        public void BreakpointContinue()
        {
            bpStall.Set();
        }

        public void RunThreaded()
        {
            WinDebug.DEBUG_EVENT DbgEvt = new WinDebug.DEBUG_EVENT();
            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;

            Parallel.ForEach(SessionEvents, Event => Event.OnDebugStart());

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

                // TODO: Stop doing this once we raise an exception
                WinDebug.NativeMethods.ContinueDebugEvent(DbgEvt.dwProcessId, DbgEvt.dwThreadId, ContinueStatus);
                ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
            }
            
            State = RunState.Ended;

            Parallel.ForEach(SessionEvents, Event => Event.OnDebugEnd());
        }

        public DebuggerSymbol ResolveSymbol(uint Address)
        {
            return SymbolSrv.FindSymbol(Address);
        }

        public DebuggerSymbol ResolveSymbol(IntPtr Address)
        {
            return ResolveSymbol((uint)Address);
        }

        public DebuggerModule ResolveModule(uint Address)
        {
            return DebugInstance.FindModuleFromAddress(Address);
        }

        public void RegisterEventInterfaces(object EventClass)
        {
            IDebuggerSessionEvents SessionListener = EventClass as IDebuggerSessionEvents;
            if(SessionListener != null ) SessionEvents.Add(SessionListener);

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

            IDebuggerFileEvents FileListener = EventClass as IDebuggerFileEvents;
            if (FileListener != null) FileEvents.Add(FileListener);
        }
    }
}
