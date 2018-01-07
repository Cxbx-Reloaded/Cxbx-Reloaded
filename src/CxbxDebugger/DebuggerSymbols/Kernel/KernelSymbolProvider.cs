// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    public class KernelProvider : DebuggerSymbolProviderBase
    {
        public void AddKernelSymbolFromMessage(DebuggerMessages.KernelPatch Message)
        {
            DebuggerSymbol NewSymbol = new DebuggerSymbol();

            NewSymbol.Name = Message.Name;
            NewSymbol.AddrBegin = (uint)Message.Address;

            AddSymbol(NewSymbol);
        }
    }
}
