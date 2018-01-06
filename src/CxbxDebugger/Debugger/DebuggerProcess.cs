// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using WinProcesses = VsChromium.Core.Win32.Processes;

namespace CxbxDebugger
{
    public class DebuggerProcess
    {
        public IntPtr Handle { get; set; }
        public uint ProcessID { get; set; }

        public List<DebuggerModule> Modules { get; set; }
        public List<DebuggerThread> Threads { get; set; }

        public DebuggerModule MainModule { get; set; }
        public DebuggerThread MainThread { get; set; }

        // Based on DebugProcess
        public DebuggerProcess()
        {
            Handle = IntPtr.Zero;
            ProcessID = 0;

            Modules = new List<DebuggerModule>();
            Threads = new List<DebuggerThread>();

            MainModule = null;
            MainThread = null;
        }

        public void Suspend()
        {
            // TODO Thread may already be in a non-suspended state

            foreach (DebuggerThread Thread in Threads)
            {
                WinProcesses.NativeMethods.SuspendThread(Thread.Handle);
            }
        }

        public void Resume()
        {
            // TODO Thread may already be in a non-suspended state

            foreach (DebuggerThread Thread in Threads)
            {
                WinProcesses.NativeMethods.ResumeThread(Thread.Handle);
            }
        }

        private byte[] ReadMemoryInternal(IntPtr Address, int Size)
        {
            byte[] buffer = new byte[Size];

            int numRead = 0;
            WinProcesses.NativeMethods.ReadProcessMemory
            (
                Handle,
                Address,
                buffer,
                4,
                out numRead
            );

            // Not handling errors for now
            return buffer;
        }

        public T ReadMemory<T>(IntPtr Address)
        {
            var size = Marshal.SizeOf(typeof(T));
            byte[] Data = ReadMemoryInternal(Address, size);

            object Value = default(T);

            TypeCode TType = Type.GetTypeCode(typeof(T));
            switch (TType)
            {
                case TypeCode.Byte:
                    Value = Data[0];
                    break;

                case TypeCode.UInt32:
                    Value = BitConverter.ToUInt32(Data, 0);
                    break;

                default:
                    throw new Exception(string.Format("Unhandled type code {0}", TType.ToString()));
            }

            return (T)Value;
        }

        private void WriteMemoryInternal(IntPtr Address, byte[] Data)
        {
            int numWritten = 0;
            WinProcesses.NativeMethods.WriteProcessMemory
                (
                    Handle,
                    Address,
                    Data,
                    (uint)Data.Length,
                    out numWritten
                );
        }

        public void WriteMemory<T>(IntPtr Address, T Value)
        {
            byte[] Data = null;

            object GenericValue = Value;

            TypeCode TType = Type.GetTypeCode(Value.GetType());
            switch (TType)
            {
                case TypeCode.Byte:
                    Data = new byte[] { (byte)GenericValue };
                    break;

                case TypeCode.UInt32:
                    Data = BitConverter.GetBytes((uint)GenericValue);
                    break;

                default:
                    throw new Exception(string.Format("Unhandled type code {0}", TType.ToString()));
            }

            if (Data != null)
            {
                WriteMemoryInternal(Address, Data);
            }
        }
    }
}
