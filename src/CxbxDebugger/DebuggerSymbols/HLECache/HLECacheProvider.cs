// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    public class HLECacheProvider : DebuggerSymbolProvider
    {
        HLECacheFile CacheFile;

        public bool Load(string FileName)
        {
            CacheFile = new HLECacheFile();
            return CacheFile.Load(FileName);
        }

        public override DebuggerSymbolResult ResolveAddress(uint Address)
        {
            var SymbolResult = new DebuggerSymbolResult();

            // TODO: Resolve me
            SymbolResult.Success = false;

            return SymbolResult;
        }
    }
}
