// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections;
using System.Collections.Generic;

namespace CxbxDebugger
{
    public class DebuggerSymbol
    {
        public string Name { get; set; } = "";

        public uint AddrBegin { get; set; } = 0;
        public uint AddrEnd { get; set; } = 0;
    }

    public class DebuggerSymbolProviderBase
    {
        // Stored in a map so the addresses are sorted
        Dictionary<uint, DebuggerSymbol> Symbols = new Dictionary<uint, DebuggerSymbol>();

        protected void AddSymbol(DebuggerSymbol Symbol)
        {
            Symbols.Add(Symbol.AddrBegin, Symbol);
        }

        protected void RemoveSymbol(DebuggerSymbol Symbol)
        {
            Symbols.Remove(Symbol.AddrBegin);
        }

        // TODO Find an inbuilt lower bound method
        public DebuggerSymbol FindSymbolFromAddress(uint Address)
        {
            uint BestAddr = 0;

            foreach (uint SymbolAddr in Symbols.Keys)
            {
                if (SymbolAddr > Address)
                    break;

                BestAddr = SymbolAddr;
            }

            if (!Symbols.ContainsKey(BestAddr))
                return null;

            return Symbols[BestAddr];
        }
    }
}
