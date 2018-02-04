// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Text;
using WinProcesses = VsChromium.Core.Win32.Processes;
using WinDebug = VsChromium.Core.Win32.Debugging;
using System.Runtime.InteropServices;
using WinLowLevel = LowLevelDesign.Win32.Windows.NativeMethods;

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

        RunState State = RunState.NotLaunched;

        DebuggerMessages.DebuggerInit InitParams = new DebuggerMessages.DebuggerInit();
        
        DebuggerInstance DebugInstance;

        List<IDebuggerGeneralEvents> GeneralEvents = new List<IDebuggerGeneralEvents>();
        List<IDebuggerProcessEvents> ProcessEvents = new List<IDebuggerProcessEvents>();
        List<IDebuggerThreadEvents> ThreadEvents = new List<IDebuggerThreadEvents>();
        List<IDebuggerModuleEvents> ModuleEvents = new List<IDebuggerModuleEvents>();
        List<IDebuggerOutputEvents> OutputEvents = new List<IDebuggerOutputEvents>();
        List<IDebuggerExceptionEvents> ExceptionEvents = new List<IDebuggerExceptionEvents>();
        List<IDebuggerFileEvents> FileEvents = new List<IDebuggerFileEvents>();

        DebuggerSymbolServer SymbolSrv;
        KernelProvider KernelSymbolProvider;

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

            // Copy XBE path without the quotes
            if(x_args.Length > 2)
            {
                Target = x_args[2].Trim(new char[] { '"' });
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
                // Store so they can be marshalled and closed correctly
                hProcess = new WinProcesses.SafeProcessHandle(stProcessInfo.hProcess);
                hThread = new WinProcesses.SafeThreadHandle(stProcessInfo.hThread);
                
                bContinue = true;

                State = RunState.Running;
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

            foreach (IDebuggerThreadEvents Event in ThreadEvents )
            {
                Event.OnThreadCreate(Thread);
            }
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

                foreach (IDebuggerThreadEvents Event in ThreadEvents)
                {
                    Event.OnThreadExit(TargetThread, ExitCode);
                }
            }
        }

        const int VM_PLACEHOLDER_SIZE = 0x3FEF000;
        const int XBE_IMAGE_BASE = 0x00010000;

        private void HandleCreateProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.CreateProcessInfo;

            var Process = new DebuggerProcess();

            Process.Core = true;
            Process.Handle = DebugInfo.hProcess;
            Process.ProcessID = WinLowLevel.GetProcessId(Process.Handle);
            Process.Path = ResolveProcessPath(DebugInfo.hFile);

            // Skip over allocated Xbox memory
            Process.ImageBase = DebugInfo.lpStartAddress + VM_PLACEHOLDER_SIZE; 

            var MainThread = new DebuggerThread(Process);

            MainThread.Handle = DebugInfo.hThread;
            MainThread.ThreadID = NativeMethods.GetThreadId(DebugInfo.hThread);
            MainThread.ThreadBase = DebugInfo.lpThreadLocalBase;

            // Setup as the main thread
            // TODO Check that we need to treat this as a special case
            Process.MainThread = MainThread;

            DebugInstance = new DebuggerInstance(Process);
            RegisterEventInterfaces(DebugInstance);

            foreach (IDebuggerProcessEvents Event in ProcessEvents)
            {
                Event.OnProcessCreate(Process);
            }

            foreach (IDebuggerThreadEvents Event in ThreadEvents)
            {
                Event.OnThreadCreate(MainThread);
            }
            
            var XboxModule = new DebuggerModule();
            
            XboxModule.Path = Target;
            XboxModule.ImageBase = new IntPtr(XBE_IMAGE_BASE);
            XboxModule.Core = true;

            foreach (IDebuggerModuleEvents Event in ModuleEvents)
            {
                Event.OnModuleLoaded(XboxModule);
            }
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
                foreach (IDebuggerProcessEvents Event in ProcessEvents)
                {
                    Event.OnProcessExit(TargetProcess, ExitCode);
                }
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

            foreach (IDebuggerModuleEvents Event in ModuleEvents)
            {
                Event.OnModuleLoaded(Module);
            }
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
                    {
                        // Ignore all access violation errors.

                        // This will fallback to the debugee (Cxbx) and the exception handling routine.
                        // Should they be unsupported, the debugger "OVERRIDE_EXCEPTION" message is thrown
                        // which means the exception is usually fatal

                        ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_EXCEPTION_NOT_HANDLED;
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.OVERRIDE_EXCEPTION:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Query = DebuggerMessages.GetExceptionHandledQuery(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);
                            
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
                            var Report = DebuggerMessages.GetHLECacheReport(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);
                            SetupHLECacheProvider(Report.FileName);
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.KERNEL_PATCH:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetKernelPatchReport(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);

                            KernelSymbolProvider.AddKernelSymbolFromMessage(Report);
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.FILE_OPENED:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetFileOpenedReport(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);

                            foreach (IDebuggerFileEvents Event in FileEvents)
                            {
                                Event.OnFileOpened(Report);
                            }
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.FILE_READ:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetFileReadReport(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);

                            foreach (IDebuggerFileEvents Event in FileEvents)
                            {
                                Event.OnFileRead(Report);
                            }
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.FILE_WRITE:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetFileWriteReport(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);

                            foreach (IDebuggerFileEvents Event in FileEvents)
                            {
                                Event.OnFileWrite(Report);
                            }
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.FILE_CLOSED:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetFileClosedReport(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);
                            if (Report != null)
                            {
                                foreach (IDebuggerFileEvents Event in FileEvents)
                                {
                                    Event.OnFileClosed(Report);
                                }
                            }
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.DEBUGGER_INIT:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetDebuggerInitReport(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);

                            InitParams = Report;
                        }
                    }
                    break;

                case (ExceptionCode)DebuggerMessages.ReportType.MS_VC_EXCEPTION:
                    {
                        var Thread = DebugInstance.MainProcess.FindThread((uint)DebugEvent.dwThreadId);
                        if (Thread != null)
                        {
                            var Report = DebuggerMessages.GetMSVCThreadName(Thread, DebugInfo.ExceptionRecord.ExceptionInformation);
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

                                    foreach (IDebuggerThreadEvents Event in ThreadEvents)
                                    {
                                        Event.OnThreadNamed(Thread);
                                    }
                                }
                            }
                        }
                    }
                    break;

                case ExceptionCode.Breakpoint:
                    // TODO Handle

                    break;

                case ExceptionCode.SingleStep:
                    // TODO Handle
                    break;

                default:
                    ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_EXCEPTION_NOT_HANDLED;
                    break;
            }
        }

        public void RunThreaded()
        {
            WinDebug.DEBUG_EVENT DbgEvt = new WinDebug.DEBUG_EVENT();
            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;

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
            return ResolveSymbol((uint)Address);
        }

        public DebuggerModule ResolveModule(uint Address)
        {
            return DebugInstance.FindModuleFromAddress(Address);
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

            IDebuggerFileEvents FileListener = EventClass as IDebuggerFileEvents;
            if (FileListener != null) FileEvents.Add(FileListener);
        }

        public IntPtr GetScreenBuffer()
        {
            return InitParams.ScreenBuffer;
        }
    }
}
