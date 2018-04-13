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
            Binary,
            Byte,
            Bytes2,
            Bytes4,
            Float,
            Double,
            String,
        }

        public struct CheatEntry
        {
            public string ID;
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
            public byte[] Before;
            public byte[] Actual;
            public byte[] After;
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
    }
}
