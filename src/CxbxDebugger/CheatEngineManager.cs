// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using CxbxDebugger.CheatEngine;

namespace CxbxDebugger
{
    class CheatEngineManager
    {
        public CheatTable Cheats;

        public string Read(DebuggerProcess Process, int CheatIndex)
        {
            string printed_value = "<failed to read>";
            CheatEntry entry = Cheats.CheatEntries[CheatIndex];

            int addr = 0;
            if (!ReadHexInt(entry.Address, ref addr))
            {
                return printed_value;
            }
            
            switch (entry.VariableType)
            {
                case Variable.Byte:
                    {
                        var data = Process.ReadMemoryBlock(new IntPtr(addr), 1);

                        if (data != null)
                        {
                            if (entry.ShowAsHex)
                            {
                                printed_value = string.Format("0x{0:x}", data[0]);
                            }
                            else
                            {
                                printed_value = string.Format("{0}", data[0]);
                            }

                        }
                        break;
                    }

                case Variable.Bytes2:
                    {
                        var data = Process.ReadMemoryBlock(new IntPtr(addr), 2);

                        if (data != null)
                        {
                            ushort val = BitConverter.ToUInt16(data, 0);

                            if (entry.ShowAsHex)
                            {
                                printed_value = string.Format("0x{0:x}", val);
                            }
                            else
                            {
                                printed_value = string.Format("{0}", val);
                            }

                        }
                        break;
                    }

                case Variable.Bytes4:
                    {
                        var data = Process.ReadMemoryBlock(new IntPtr(addr), 4);

                        if (data != null)
                        {
                            uint val = BitConverter.ToUInt32(data, 0);

                            if (entry.ShowAsHex)
                            {
                                printed_value = string.Format("0x{0:x}", val);
                            }
                            else
                            {
                                printed_value = string.Format("{0}", val);
                            }

                        }
                        break;
                    }

                case Variable.Float:
                    {
                        var data = Process.ReadMemoryBlock(new IntPtr(addr), 4);
                        if (data != null)
                        {
                            if (entry.ShowAsHex)
                            {
                                uint val = BitConverter.ToUInt32(data, 0);
                                printed_value = string.Format("{0:x8}", val);
                            }
                            else
                            {
                                float flt = BitConverter.ToSingle(data, 0);
                                printed_value = string.Format("{0}", flt);
                            }

                        }
                        break;
                    }
            }

            return printed_value;
        }

        public void Write(DebuggerProcess Process, int CheatIndex, string Value)
        {
            CheatEntry entry = Cheats.CheatEntries[CheatIndex];

            int addr = 0;
            if (!ReadHexInt(entry.Address, ref addr))
            {
                return;
            }

            switch (entry.VariableType)
            {
                case Variable.Byte:
                    {
                        uint byte_val = 0;
                        ReadAddress(Value, ref byte_val);

                        byte val = (byte)(byte_val & 0xFF);
                        Process.WriteMemory(new IntPtr(addr), val);

                        break;
                    }
            }
        }

        static private bool ReadHexInt(string HexSource, ref int Out)
        {
            if (int.TryParse(HexSource, System.Globalization.NumberStyles.HexNumber, null, out Out))
            {
                return true;
            }

            return false;
        }


        static private bool ReadAddress(string Source, ref uint Out)
        {
            try
            {
                if (Source.StartsWith("0x"))
                {
                    Out = Convert.ToUInt32(Source.Substring(2), 16);
                    return true;
                }
                else
                {
                    if (uint.TryParse(Source, out Out))
                    {
                        return true;
                    }
                }
            }
            catch (Exception) { }

            return false;
        }
    }
}
