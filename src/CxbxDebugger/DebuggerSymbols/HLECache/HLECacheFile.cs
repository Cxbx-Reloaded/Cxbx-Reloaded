// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;

namespace CxbxDebugger
{
    public class HLECacheFile
    {
        public Dictionary<uint, string> AddressMap { get; private set; }

        public bool Load(string FileName)
        {
            var iniReader = new Utils.INIReader(FileName);
            if (!iniReader.IsValid)
                return false;

            var Pairs = iniReader.GetSectionContentsPairs("Symbols");
            if (Pairs == null)
                return false;

            AddressMap = new Dictionary<uint, string>(Pairs.Length);

            // Swap SYMBOL, ADDR to be ADDR -> SYMBOL
            foreach (KeyValuePair<string, string> Symbol in Pairs)
            {
                uint Address = Convert.ToUInt32(Symbol.Value);
                AddressMap.Add(Address, Symbol.Key);
            }

            return true;
        }

    }
}
