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
    public class Stackframe
    {
        public IntPtr BasePointer { get; }
        public IntPtr CodeAddress { get; }

        public Stackframe(IntPtr Base, IntPtr CodeAddr)
        {
            BasePointer = Base;
            CodeAddress = CodeAddr;
        }
    }

    public class Debugger : IDisposable
    {
        // Cached process information
        WinProcesses.PROCESS_INFORMATION stProcessInfo = new WinProcesses.PROCESS_INFORMATION();
        WinProcesses.STARTUPINFO stStartInfo = new WinProcesses.STARTUPINFO();
        
        // Wrapped so these can be closed automatically
        WinProcesses.SafeProcessHandle hProcess = new WinProcesses.SafeProcessHandle();
        WinProcesses.SafeThreadHandle hThread = new WinProcesses.SafeThreadHandle();

        string[] args = new string[] { };
        bool Running = false;

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

        public bool Launch()
        {
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

                Running = true;
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

            // TODO: Expose as variable
            const int MaxFrames = 16;

            var Callstack = new List<Stackframe>();
            var ebp = lastReadCtxt.ebp;
            
            Callstack.Add(new Stackframe(new IntPtr(ebp), new IntPtr(lastReadCtxt.eip)));

            do
            {
                var ReturnAddr = GetMemory(new IntPtr(ebp + 4));
                ebp = GetMemory(new IntPtr(ebp));

                if (ebp == 0 || ReturnAddr == ebp)
                    break;

                Callstack.Add(new Stackframe(new IntPtr(ebp), new IntPtr(ReturnAddr)));
            }
            while (Callstack.Count < MaxFrames);


            var strList = new List<string>(Callstack.Count);

            foreach (Stackframe sf in Callstack)
            {
                // TODO: Resolve BasePointer to symbol database
                strList.Add(string.Format("{0:X8} and {1:X8}", (uint)sf.BasePointer, (uint)sf.CodeAddress));
            }

            foreach (DebuggerEventListener EvtListener in EventListeners)
            {
                EvtListener.OnCallstack(strList.ToArray());
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
                            // TODO: Handle properly, storing process info

                            foreach (DebuggerEventListener EvtListener in EventListeners)
                            {
                                EvtListener.OnProcessCreate();
                            }

                            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.EXIT_PROCESS_DEBUG_EVENT:
                        {
                            foreach (DebuggerEventListener EvtListener in EventListeners)
                            {
                                EvtListener.OnProcessExit();
                            }

                            bContinue = false;
                            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.LOAD_DLL_DEBUG_EVENT:
                        {
                            // TODO: Handle properly

                            string Name = ResolveProcessPath(DbgEvt.LoadDll.hFile);
                            foreach (DebuggerEventListener EvtListener in EventListeners)
                            {
                                EvtListener.OnModuleLoaded(Name);
                            }

                            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.UNLOAD_DLL_DEBUG_EVENT:
                        {
                            // TODO: Handle properly, looking up module being unloaded

                            foreach (DebuggerEventListener EvtListener in EventListeners)
                            {
                                EvtListener.OnModuleUnloaded();
                            }
                            
                            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.OUTPUT_DEBUG_STRING_EVENT:
                        {
                            string dbgStr = ReadProcessString(DbgEvt.DebugString.lpDebugStringData, DbgEvt.DebugString.nDebugStringLength, DbgEvt.DebugString.fUnicode == 1);
                            //Console.WriteLine(dbgStr);

                            foreach(DebuggerEventListener EvtListener in EventListeners)
                            {
                                EvtListener.OnDebugOutput(dbgStr);
                            }

                            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
                        }
                        break;

                    case WinDebug.DEBUG_EVENT_CODE.EXCEPTION_DEBUG_EVENT:
                        {
                            // TODO: Handle properly

                            BuildCallstack();

                            // Temporary. Should wait until owning systems are happy to continue
                            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
                        }
                        break;

                    default:
                        {
                            // For any other events, just continue
                            ContinueStatus = WinDebug.CONTINUE_STATUS.DBG_CONTINUE;
                        }
                        break;
                }

                // Continue on our merry way
                // TODO: Wait until our handlers are happy to continue
                WinDebug.NativeMethods.ContinueDebugEvent(DbgEvt.dwProcessId, DbgEvt.dwThreadId, ContinueStatus);
            }

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
