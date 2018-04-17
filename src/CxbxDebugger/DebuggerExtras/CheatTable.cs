// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;

namespace CxbxDebugger
{
    namespace CheatEngine
    {
        public struct State
        {
            public string Value;
            public bool Activated;
            public uint RealAddress;
        };

        public enum Variable
        {
            Unsupported,
            Byte,
            Bytes2,
            Bytes4,
            Float,
            Double,
        }

        public struct CheatEntry
        {
            public uint ID;
            public string Description;
            public bool ShowAsHex;
            public State LastState;
            public uint Color;
            public Variable VariableType;
            public string Address;
        };

        public struct CodeEntry
        {
            public string Description;
            public uint Address;
            public string ModuleName;
            public uint ModuleNameOffset;
            public byte[] Before; // 5 bytes before 'Actual'
            public byte[] Actual; // with a length of the opcode
            public byte[] After; // 5 bytes after 'Actual'
        };

        public class CheatTable
        {
            public string Comments;
            public List<CheatEntry> CheatEntries;
            public List<CodeEntry> CodeEntires;

            public CheatTable()
            {
                Comments = "";
                CheatEntries = new List<CheatEntry>();
                CodeEntires = new List<CodeEntry>();
            }
        };

        class Helpers
        {
            public static int VariableSize(Variable Var)
            {
                switch (Var)
                {
                    case Variable.Unsupported:
                        return 0;
                    case Variable.Byte:
                        return 1;
                    case Variable.Bytes2:
                        return 2;
                    case Variable.Bytes4:
                        return 4;
                    case Variable.Float:
                        return 4;
                    case Variable.Double:
                        return 8;
                }

                return 0;
            }
        }
    }
}
