// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;

namespace CxbxDebugger
{
    public class DebuggerSymbolServer
    {
        List<DebuggerSymbolProviderBase> Providers = new List<DebuggerSymbolProviderBase>();

        public DebuggerSymbol FindSymbol(uint Address)
        {
            if (Providers.Count == 0)
                return null;

            DebuggerSymbol BestResult = null;

            // Find the first provider who can give a symbol for this address

            int ProviderIndex = 0;
            do
            {
                BestResult = Providers[ProviderIndex].FindSymbolFromAddress(Address);
                if (BestResult != null)
                    break;

                ++ProviderIndex;
            }
            while (ProviderIndex < Providers.Count);

            // Failed to find any symbols
            if( BestResult == null )
                return null;

            // Found an exact match
            if (BestResult.AddrBegin == Address)
                return BestResult;

            // Otherwise, try and find another symbol which is closer to the given address
            // TODO Investigate adding symbol size metadata this can just check if Address is within a given range

            for (int i = ProviderIndex + 1; i < Providers.Count; ++i)
            {
                DebuggerSymbol OtherResult = Providers[i].FindSymbolFromAddress(Address);
                if (OtherResult == null)
                    continue;

                if(OtherResult.AddrBegin < BestResult.AddrBegin )
                {
                    BestResult = OtherResult;
                }
            }

            return BestResult;
        }

        public void RegisterProvider(DebuggerSymbolProviderBase Provider)
        {
            Providers.Add(Provider);
        }

        public void UnregisterProvider(DebuggerSymbolProviderBase Provider)
        {
            Providers.Remove(Provider);
        }
    }
}
