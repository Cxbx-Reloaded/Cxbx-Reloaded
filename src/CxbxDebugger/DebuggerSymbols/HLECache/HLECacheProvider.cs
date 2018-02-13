// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;
using System.IO;

namespace CxbxDebugger
{
    public class HLECacheProvider : DebuggerSymbolProviderBase
    {
        public bool Load(string FileName)
        {
            if (!File.Exists(FileName))
                return false;

            HLECacheFile CacheFile = new HLECacheFile();
            if (!CacheFile.Load(FileName))
                return false;

            foreach(KeyValuePair<uint, string> Item in CacheFile.AddressMap)
            {
                DebuggerSymbol NewSymbol = new DebuggerSymbol();
                NewSymbol.Name = Item.Value;
                NewSymbol.AddrBegin = Item.Key;

                AddSymbol(NewSymbol);
            }

            return true;
        }
    }
}
