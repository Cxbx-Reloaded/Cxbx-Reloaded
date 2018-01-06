// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    public class KernelProvider : DebuggerSymbolProviderBase
    {
        public void AddKernelSymbolFromMessage(string Message)
        {
            var Parts = Message.Split('@');
            if( Parts.Length == 2 )
            {
                uint Address = 0;
                if( uint.TryParse(Parts[1], out Address) )
                {
                    DebuggerSymbol NewSymbol = new DebuggerSymbol();
                    NewSymbol.Name = Parts[0];
                    NewSymbol.AddrBegin = Address;

                    AddSymbol(NewSymbol);
                }
            }
        }
    }
}
