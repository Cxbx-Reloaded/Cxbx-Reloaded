// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;

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
                        Result = string.Format("{0:X2}", Data[0]);
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
            IntPtr PatchAddr = new IntPtr(PatchItem.Offset);

            switch (PatchItem.DisplayAs)
            {
                case PatchType.Byte:
                    {
                        byte Value = 0;
                        if (Common.ReadByte(NewValue, ref Value))
                        {
                            OwningProcess.WriteMemory(PatchAddr, Value);
                            return true;
                        }
                        break;
                    }

                case PatchType.Short:
                    {
                        short Value;
                        if (short.TryParse(NewValue, out Value))
                        {
                            OwningProcess.WriteMemory(PatchAddr, Value);
                            return true;
                        }
                        break;
                    }
                case PatchType.UShort:
                    {
                        ushort Value;
                        if (ushort.TryParse(NewValue, out Value))
                        {
                            OwningProcess.WriteMemory(PatchAddr, Value);
                            return true;
                        }
                        break;
                    }
                case PatchType.Int:
                    {
                        int Value;
                        if (int.TryParse(NewValue, out Value))
                        {
                            OwningProcess.WriteMemory(PatchAddr, Value);
                            return true;
                        }
                        break;
                    }
                case PatchType.UInt:
                    {
                        uint Value;
                        if (uint.TryParse(NewValue, out Value))
                        {
                            OwningProcess.WriteMemory(PatchAddr, Value);
                            return true;
                        }
                        break;
                    }
                case PatchType.Float:
                    {
                        float Value;
                        if (float.TryParse(NewValue, out Value))
                        {
                            OwningProcess.WriteMemory(PatchAddr, Value);
                            return true;
                        }
                        break;
                    }
                case PatchType.Array:
                    {
                        List<byte> ParsedBytes = new List<byte>();

                        string[] Items = NewValue.Split(new char[] { ' ' });
                        foreach (string Item in Items)
                        {
                            byte Value = 0;
                            if (Common.ReadByte(Item, ref Value))
                            {
                                ParsedBytes.Add(Value);
                            }
                        }
                        if (ParsedBytes.Count == PatchItem.Patched.Length)
                        {
                            OwningProcess.WriteMemoryBlock(PatchAddr, ParsedBytes.ToArray());
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
