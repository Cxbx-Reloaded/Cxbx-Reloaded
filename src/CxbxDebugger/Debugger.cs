// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Text;
using WinProcesses = VsChromium.Core.Win32.Processes;
using WinDebug = VsChromium.Core.Win32.Debugging;
using System.Runtime.InteropServices;
using CxbxDebugger.TheadHelpers;

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

        RunState State = RunState.NotLaunched;

        List<DebuggerModule> ModuleList = new List<DebuggerModule>();
        List<DebuggerThread> ThreadList = new List<DebuggerThread>();
        List<DebuggerProcess> ProcessList = new List<DebuggerProcess>();
        
        List<DebuggerEventListener> EventListeners = new List<DebuggerEventListener>();

        CONTEXT_x86 lastReadCtxt = new CONTEXT_x86
        {
            ContextFlags =
                ContextFlags.CONTEXT_i386 |
                ContextFlags.CONTEXT_CONTROL |
                ContextFlags.CONTEXT_INTEGER |
                ContextFlags.CONTEXT_SEGMENTS
        };
        
        public Debugger()
        {
            // 
        }

        public Debugger(string[] x_args)
        {
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
            // This will ONLY suspend the main process
            // Not ideal if there are multiple processes in play

            if (!hThread.IsInvalid)
            {
                WinProcesses.NativeMethods.SuspendThread(hThread.DangerousGetHandle());
            }
        }

        public void Resume()
        {
            if (!hThread.IsInvalid)
            {
                WinProcesses.NativeMethods.ResumeThread(hThread.DangerousGetHandle());
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
                hProcess = new WinProcesses.SafeProcessHandle(stProcessInfo.hProcess);
                hThread = new WinProcesses.SafeThreadHandle(stProcessInfo.hThread);

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
            if (Length == 0 || Length > 512)
                return "";

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
                    return Encoding.Unicode.GetString(buffer, 0, (int)numRead);
                }
                else
                { 
                    return Encoding.ASCII.GetString(buffer, 0, (int)numRead);
                }
            }

            return "";
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

        private void BuildCallstack()
        {
            // Skip if we have nothing to report to
            if (EventListeners.Count == 0)
                return;

            bool Result = NativeMethods.GetThreadContext(hThread.DangerousGetHandle(), ref lastReadCtxt);
            if (!Result)
            {
                // TODO: Handle this
                return;
            }

            var ebp = lastReadCtxt.ebp;

            var Callstack = new DebuggerCallstack();
            Callstack.AddFrame(new DebuggerStackFrame(new IntPtr(ebp), new IntPtr(lastReadCtxt.eip)));
            
            do
            {
                var ReturnAddr = GetMemory(new IntPtr(ebp + 4));
                ebp = GetMemory(new IntPtr(ebp));

                if (ebp == 0 || ReturnAddr == ebp)
                    break;

                Callstack.AddFrame(new DebuggerStackFrame(new IntPtr(ebp), new IntPtr(ReturnAddr)));
            }
            while (!Callstack.HasEnoughFrames);

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnCallstack(Callstack);
            }
        }

        private void HandleCreateProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            // TODO Investigate why this is throwing an exception converting to this type
            //var DebugInfo = DebugEvent.CreateProcessInfo;
            
            var Process = new DebuggerProcess();

            //DebugInfo.hFile
            //DebugInfo.hProcess
            //DebugInfo.hThread

            //DebugInfo.lpBaseOfImage
            //DebugInfo.lpStartAddress

            ProcessList.Add(Process);

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnProcessCreate(Process);
            }
        }

        private void HandleExitProcess(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.ExitProcess;

            //DebugInfo.dwExitCode

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

            ModuleList.Add(Module);

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnModuleLoaded(Module);
            }
        }

        private void HandleUnloadDll(WinDebug.DEBUG_EVENT DebugEvent)
        {
            var DebugInfo = DebugEvent.UnloadDll;

            //DebugInfo.lpBaseOfDll

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnModuleUnloaded(null);
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
            WinDebug.DEBUG_EVENT DbgEvt;
            
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
                    case WinDebug.DEBUG_EVENT_CODE.CREATE_PROCESS_DEBUG_EVENT:
                        {
                            HandleCreateProcess(DbgEvt);
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.EXIT_PROCESS_DEBUG_EVENT:
                        {
                            HandleExitProcess(DbgEvt);

                            // XX temp
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

                    case WinDebug.DEBUG_EVENT_CODE.EXCEPTION_DEBUG_EVENT:
                        {
                            // TODO: Handle properly

                            BuildCallstack();
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
