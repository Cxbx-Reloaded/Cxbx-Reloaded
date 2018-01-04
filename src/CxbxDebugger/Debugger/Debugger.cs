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
    public class Debugger : DebuggerEventListener, IDisposable
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

        List<DebuggerEventListener> EventListeners = new List<DebuggerEventListener>();

        DebuggerSymbolServer SymbolSrv;

        private void Init()
        {
            DebugInstance = null;

            SymbolSrv = new DebuggerSymbolServer();
            
            RegisterEventListener(this);
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


        public override void OnDebugOutput(string Message)
        {
            if (Message.StartsWith(CxbxDebuggerPrefix))
            {
                SetupHLECacheProvider(Message.Substring(CxbxDebuggerPrefix.Length));
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

                Process.Threads.Add(Thread);
                Process.MainThread = Thread;

                DebugInstance = new DebuggerInstance(Process);

                // Register the instance to track thread creation
                RegisterEventListener(DebugInstance);

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

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnThreadCreate(Thread);
            }
        }
        
        private void HandleExitThread(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.ExitThread;
            
            var TargetThread = DebugInstance.MainProcess.Threads.Find(Thread => Thread.ThreadID == DebugEvent.dwThreadId);
            //uint ExitCode = DebugInfo.dwExitCode;

            if (TargetThread != null)
            {
                foreach (DebuggerEventListener EvtListener in EventListeners)
                {
                    EvtListener.OnThreadExit(TargetThread);
                }
            }
        }

        private void HandleCreateProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.CreateProcessInfo;

            // Stub

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnProcessCreate(null);
            }
        }

        private void HandleExitProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.ExitProcess;

            // Stub

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnProcessExit(null);
            }
        }

        private void HandleLoadDll(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.LoadDll;

            var Module = new DebuggerModule();
            
            Module.Path = ResolveProcessPath(DebugInfo.hFile);
            Module.ImageBase = DebugInfo.lpBaseOfDll;

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnModuleLoaded(Module);
            }
        }

        private void HandleUnloadDll(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.UnloadDll;

            var TargetModule = DebugInstance.MainProcess.Modules.Find(Module => Module.ImageBase == DebugInfo.lpBaseOfDll);

            if (TargetModule != null)
            {
                foreach (DebuggerEventListener EvtListener in EventListeners)
                {
                    EvtListener.OnModuleUnloaded(TargetModule);
                }
            }
        }

        private void HandleOutputDebugString(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.DebugString;

            string debugString = ReadProcessString(DebugInfo.lpDebugStringData, DebugInfo.nDebugStringLength, DebugInfo.fUnicode == 1);

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnDebugOutput(debugString);
            }
        }
        
        public void Run()
        {
            WinDebug.DEBUG_EVENT DbgEvt = new WinDebug.DEBUG_EVENT();
            
            WinDebug.CONTINUE_STATUS ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
            bool bContinue = true;

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnDebugStart();
            }

            // Loop until told to stop
            while (bContinue == true)
            {
                // Pause until a debug event notification happens
                bContinue = WinDebug.NativeMethods.WaitForDebugEvent(out DbgEvt, VsChromium.Core.Win32.Constants.INFINITE);

                switch (DbgEvt.dwDebugEventCode)
                {
                    case WinDebug.DEBUG_EVENT_CODE.EXCEPTION_DEBUG_EVENT:
                        {
                            // TODO: Handle
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.CREATE_THREAD_DEBUG_EVENT:
                        {
                            HandleCreateThread(DbgEvt);
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.CREATE_PROCESS_DEBUG_EVENT:
                        {
                            // TODO: Limit support for multiple processes
                            HandleCreateProcess(DbgEvt);
                        }
                        break;


                    case WinDebug.DEBUG_EVENT_CODE.EXIT_THREAD_DEBUG_EVENT:
                        {
                            HandleExitThread(DbgEvt);
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.EXIT_PROCESS_DEBUG_EVENT:
                        {
                            HandleExitProcess(DbgEvt);

                            // XX Temporary
                            bContinue = false;
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.LOAD_DLL_DEBUG_EVENT:
                        {
                            HandleLoadDll(DbgEvt);
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.UNLOAD_DLL_DEBUG_EVENT:
                        {
                            HandleUnloadDll(DbgEvt);
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.OUTPUT_DEBUG_STRING_EVENT:
                        {
                            HandleOutputDebugString(DbgEvt);
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.RIP_EVENT:
                        {
                            // TODO: Handle
                        }
                        break;
                }

                WinDebug.NativeMethods.ContinueDebugEvent(DbgEvt.dwProcessId, DbgEvt.dwThreadId, ContinueStatus);
            }
            
            State = RunState.Ended;

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnDebugEnd();
            }
        }

        public void RegisterEventListener(DebuggerEventListener EventListener)
        {
            EventListeners.Add(EventListener);
        }

        public void UnregisterEventListener(DebuggerEventListener EventListener)
        {
            EventListeners.Remove(EventListener);
        }
    }
}
