// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.IO;

namespace CxbxDebugger
{
    enum PatchType
    {
        Byte,
        Short,
        UShort,
        Int,
        UInt,
        Float,
        Array
    };

    struct Patch
    {
        public PatchType DisplayAs { get; set; }
        public string Name { get; set; }
        public string Module { get; set; }
        public uint Offset { get; set; }
        public byte[] Original { get; set; }
        public byte[] Patched { get; set; }
    };

    class PatchManager : IDebuggerProcessEvents, IDebuggerModuleEvents
    {
        public List<Patch> Data = new List<Patch>();
        public List<Patch> Assembly = new List<Patch>();

        public void OnProcessCreate(DebuggerProcess Process)
        {
            // TODO: apply pending patches
        }

        public void OnProcessExit(DebuggerProcess Process, uint ExitCode) { }

        public void OnModuleLoaded(DebuggerModule Module)
        {
            // TODO: apply pending patches
        }

        public void OnModuleUnloaded(DebuggerModule Module) { }

        static int PatchSize(Patch PatchItem)
        {
            if (PatchItem.Original != null)
                return PatchItem.Original.Length;

            if (PatchItem.Patched != null)
                return PatchItem.Patched.Length;

            return PatchTypeLength(PatchItem.DisplayAs);
        }

        private static string Format(PatchType Type, byte[] Data)
        {
            string Result = "";

            switch (Type)
            {
                case PatchType.Byte:
                    {
                        Result = string.Format("{0}", Data[0]);
                    }
                    break;
                case PatchType.Short:
                    {
                        var Value = BitConverter.ToInt16(Data, 0);
                        Result = string.Format("{0}", Value);
                    }
                    break;
                case PatchType.UShort:
                    {
                        var Value = BitConverter.ToUInt16(Data, 0);
                        Result = string.Format("{0}", Value);
                    }
                    break;
                case PatchType.Int:
                    {
                        var Value = BitConverter.ToInt32(Data, 0);
                        Result = string.Format("{0}", Value);
                    }
                    break;
                case PatchType.UInt:
                    {
                        var Value = BitConverter.ToUInt32(Data, 0);
                        Result = string.Format("{0}", Value);
                    }
                    break;
                case PatchType.Float:
                    {
                        var Value = BitConverter.ToSingle(Data, 0);
                        Result = string.Format("{0}", Value);
                    }
                    break;
                case PatchType.Array:
                    {
                        foreach (byte B in Data)
                        {
                            Result += string.Format("{0:X2} ", B);
                        }
                    }
                    break;
            }

            return Result;
        }

        public string Read(DebuggerProcess OwningProcess, Patch PatchItem)
        {
            int Size = PatchSize(PatchItem);
            if (Size == 0)
                return "";

            byte[] Current = OwningProcess.ReadMemoryBlock(new IntPtr(PatchItem.Offset), (uint)Size);
            return Format(PatchItem.DisplayAs, Current);
        }

        public bool Write(DebuggerProcess OwningProcess, Patch PatchItem, string NewValue)
        {
            switch (PatchItem.DisplayAs)
            {
                case PatchType.Byte:
                    {
                        byte Value;
                        if( byte.TryParse(NewValue, out Value) )
                        {
                            OwningProcess.WriteMemory(new IntPtr(PatchItem.Offset), Value);
                            return true;
                        }
                        break;
                    }
            }

            return false;
        }

        static public int PatchTypeLength(PatchType Type)
        {
            switch (Type)
            {
                case PatchType.Byte:
                    return 1;
                case PatchType.Short:
                    return 2;
                case PatchType.UShort:
                    return 2;
                case PatchType.Int:
                    return 4;
                case PatchType.UInt:
                    return 4;
                case PatchType.Float:
                    return 4;
            }

            return 0;
        }
    }
}
