// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;

namespace CxbxDebugger
{
    namespace CheatEngine
    {
        public struct State
        {
            public uint Value;
            public bool Activated;
            public uint RealAddress;
        };

        public enum Variable
        {
            Unknown,
            Bytes4,
        }

        public struct CheatEntry
        {
            public string ID;
            public string Description;
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
            public List<CheatEntry> CheatEntries;
            public List<CodeEntry> CodeEntires;

            public CheatTable()
            {
                CheatEntries = new List<CheatEntry>();
                CodeEntires = new List<CodeEntry>();
            }
        };
    }
}
