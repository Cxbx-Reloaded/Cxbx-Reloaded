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
        public List<DebuggerModule> Modules { get; private set; }
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

        public void Trace()
        {
            foreach (DebuggerThread Thread in Threads)
            {
                Thread.UpdateContext();
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
                // A read error could be a malformed address, so fail gracefully
                return null;
            }

            // Not handling errors for now
            return buffer;
        }

        public bool ReadMemory<T>(IntPtr Address, ref T Result)
        {
            if (Address == IntPtr.Zero)
                return false;

            TypeCode TType = Type.GetTypeCode(typeof(T));

            if (TType == TypeCode.String)
                throw new Exception("ReadMemory does not support reading strings of unknown length!");

            uint Size = (uint)Marshal.SizeOf(typeof(T));
            byte[] Data = ReadMemoryInternal(Address, Size);

            if (Data == null)
                return false;

            object Value = default(T);

            switch (TType)
            {
                case TypeCode.Byte:
                    Value = Data[0];
                    break;

                case TypeCode.Int16:
                    Value = BitConverter.ToInt16(Data, 0);
                    break;

                case TypeCode.UInt16:
                    Value = BitConverter.ToUInt16(Data, 0);
                    break;

                case TypeCode.Int32:
                    Value = BitConverter.ToInt32(Data, 0);
                    break;

                case TypeCode.UInt32:
                    Value = BitConverter.ToUInt32(Data, 0);
                    break;

                case TypeCode.Single:
                    Value = BitConverter.ToSingle(Data, 0);
                    break;

                default:
                    throw new Exception(string.Format("Unhandled type code {0}", TType.ToString()));
            }

            Result = (T)Value;
            return true;
        }

        public string ReadString(IntPtr Address)
        {
            if (Address == IntPtr.Zero)
                return "";

            List<byte> StringData = new List<byte>();

            byte chr = 0;
            while (true)
            {
                if (!ReadMemory(Address, ref chr))
                    break;

                if (chr == 0)
                    break;

                Address += sizeof(byte);
                StringData.Add(chr);
            }

            return Encoding.ASCII.GetString(StringData.ToArray());
        }

        public string ReadWString(IntPtr Address)
        {
            if (Address == IntPtr.Zero)
                return "";

            List<byte> StringData = new List<byte>();

            byte chr1 = 0;
            byte chr2 = 0;
            while (true)
            {
                if (!ReadMemory(Address, ref chr1))
                    break;
                if (!ReadMemory(Address + 1, ref chr2))
                    break;

                if (chr1 == 0 && chr2 == 0)
                    break;

                Address += sizeof(short);
                StringData.Add(chr1);
                StringData.Add(chr2);
            }

            return Encoding.Unicode.GetString(StringData.ToArray());
        }

        public string ReadString(IntPtr Address, uint Length)
        {
            if (Address == IntPtr.Zero)
                return "";

            byte[] StringData = ReadMemoryInternal(Address, Length);

            if (StringData == null)
                return "";

            return Encoding.ASCII.GetString(StringData);
        }

        public string ReadWString(IntPtr Address, uint Length)
        {
            if (Address == IntPtr.Zero)
                return "";

            byte[] StringData = ReadMemoryInternal(Address, Length * 2);

            if (StringData == null)
                return "";

            return Encoding.Unicode.GetString(StringData);
        }

        public byte[] ReadMemoryBlock(IntPtr Address, uint Size)
        {
            if (Address == IntPtr.Zero)
                return null;

            return ReadMemoryInternal(Address, Size);
        }

        private bool WriteMemoryInternal(IntPtr Address, byte[] Data)
        {
            int numWritten = 0;
            return WinProcesses.NativeMethods.WriteProcessMemory
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
                case TypeCode.Boolean:
                    Data = new byte[] { (bool)GenericValue ? (byte)0x1 : (byte)0x0 };
                    break;

                case TypeCode.Byte:
                    Data = new byte[] { (byte)GenericValue };
                    break;

                case TypeCode.Int16:
                    Data = BitConverter.GetBytes((short)GenericValue);
                    break;

                case TypeCode.UInt16:
                    Data = BitConverter.GetBytes((ushort)GenericValue);
                    break;

                case TypeCode.Int32:
                    Data = BitConverter.GetBytes((int)GenericValue);
                    break;

                case TypeCode.UInt32:
                    Data = BitConverter.GetBytes((uint)GenericValue);
                    break;

                case TypeCode.Single:
                    Data = BitConverter.GetBytes((float)GenericValue);
                    break;

                default:
                    throw new Exception(string.Format("Unhandled type code {0}", TType.ToString()));
            }

            if (Data != null)
            {
                WriteMemoryInternal(Address, Data);
            }
        }

        public bool WriteMemoryBlock(IntPtr Address, byte[] Data)
        {
            if (Address == IntPtr.Zero)
                return false;

            return WriteMemoryInternal(Address, Data);
        }
    }
}
