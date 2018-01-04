// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;

namespace CxbxDebugger
{
    public class DebuggerSymbolServer
    {
        List<DebuggerSymbolProvider> Providers = new List<DebuggerSymbolProvider>();

        public DebuggerSymbolServer() { }

        public DebuggerSymbolResult ResolveAddress(uint Address)
        {
            DebuggerSymbolResult Result = new DebuggerSymbolResult();

            foreach (DebuggerSymbolProvider SymbolProvider in Providers)
            {
                Result = SymbolProvider.ResolveAddress(Address);

                if (Result.Success)
                    break;
            }

            return Result;
        }

        public void RegisterProvider(DebuggerSymbolProvider Provider)
        {
            Providers.Add(Provider);
        }

        public void UnregisterProvider(DebuggerSymbolProvider Provider)
        {
            Providers.Remove(Provider);
        }
    }
}
