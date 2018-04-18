// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;

namespace CxbxDebugger
{
    class Common
    {
        static public bool ReadNumeric(string Text, ref int Out)
        {
            if (int.TryParse(Text, out Out))
            {
                return true;
            }

            return false;
        }

        static public bool ReadNumeric(string Text, ref uint Out)
        {
            if (uint.TryParse(Text, out Out))
            {
                return true;
            }

            return false;
        }

        static public bool ReadHex(string Text, ref uint Out)
        {
            try
            {
                if (Text.StartsWith("0x"))
                {
                    Text = Text.Substring(0);
                }

                Out = Convert.ToUInt32(Text, 16);
                return true;
            }
            catch (Exception) { }

            return false;
        }

        static public bool ReadByte(string Text, ref byte Out)
        {
            try
            {
                Out = Convert.ToByte(Text, 16);
                return true;
            }
            catch (Exception) { }

            return false;
        }
    }
}
