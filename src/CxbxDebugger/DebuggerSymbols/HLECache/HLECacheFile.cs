// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Globalization;

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
            uint Addr;
            foreach (KeyValuePair<string, string> Symbol in Pairs)
            {
                if (!uint.TryParse(Symbol.Value, NumberStyles.HexNumber, CultureInfo.CurrentCulture, out Addr))
                    continue;

                // See https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/832
                if (!AddressMap.ContainsKey(Addr))
                {
                    AddressMap.Add(Addr, Symbol.Key);
                }
            }

            return true;
        }

    }
}