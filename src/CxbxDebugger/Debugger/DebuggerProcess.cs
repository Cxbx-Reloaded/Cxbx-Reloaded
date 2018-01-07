// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using WinProcesses = VsChromium.Core.Win32.Processes;

namespace CxbxDebugger
{
    public class DebuggerProcess : DebuggerModule
    {
        public IntPtr Handle { get; set; }
        public uint ProcessID { get; set; }
        public List<DebuggerModule> Modules { get; set; }
        public List<DebuggerThread> Threads { get; set; }

        public DebuggerThread MainThread { get; set; }

        // Based on DebugProcess
        public DebuggerProcess()
        {
            Handle = IntPtr.Zero;
            ProcessID = 0;
            ImageBase = IntPtr.Zero;
            Path = "";

            Modules = new List<DebuggerModule>();
            Threads = new List<DebuggerThread>();

            MainThread = null;
        }

        public void Suspend()
        {
            foreach (DebuggerThread Thread in Threads)
            {
                Thread.Suspend();
            }
        }

        public void Resume()
        {
            foreach (DebuggerThread Thread in Threads)
            {
                Thread.Resume();
            }
        }

        public DebuggerThread FindThread(uint ThreadID)
        {
            return Threads.Find(Thread => Thread.ThreadID == ThreadID);
        }

        private byte[] ReadMemoryInternal(IntPtr Address, uint Size)
        {
            byte[] buffer = new byte[Size];

            int numRead = 0;
            if (!WinProcesses.NativeMethods.ReadProcessMemory
            (
                Handle,
                Address,
                buffer,
                Size,
                out numRead
            ))
            {
                throw new Exception(string.Format("ReadProcessMemory failed to read 0x{0:X8}", (uint)Address));
            }

            // Not handling errors for now
            return buffer;
        }

        public T ReadMemory<T>(IntPtr Address)
        {
            object Value = default(T);

            if (Address == IntPtr.Zero)
                return (T)Value;

            TypeCode TType = Type.GetTypeCode(typeof(T));

            if (TType == TypeCode.String)
                throw new Exception("ReadMemory does not support reading strings of unknown length!");

            uint Size = (uint)Marshal.SizeOf(typeof(T));
            byte[] Data = ReadMemoryInternal(Address, Size);
            
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

        //public string ReadString(IntPtr Address, bool IsWide)
        //{
        //    if (Address == IntPtr.Zero)
        //        return "";

        //    List<byte> StringData = new List<byte>();

        //    while(true)
        //    {
        //        byte b = ReadMemory<byte>(Address);
        //        if (b == 0)
        //            break;

        //        Address = new IntPtr((uint)Address + 1);
        //        StringData.Add(b);
        //    }

        //    Encoding StringEncoding = (IsWide ? Encoding.Unicode : Encoding.ASCII);
        //    return StringEncoding.GetString(StringData.ToArray());
        //}

        public string ReadString(IntPtr Address, uint Length)
        {
            if (Address == IntPtr.Zero)
                return "";

            byte[] StringData = ReadMemoryInternal(Address, Length);
            
            return Encoding.ASCII.GetString(StringData);
        }

        public string ReadWString(IntPtr Address, uint Length)
        {
            if (Address == IntPtr.Zero)
                return "";

            byte[] StringData = ReadMemoryInternal(Address, Length);
            
            return Encoding.Unicode.GetString(StringData);
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
