// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;
using System.Windows.Forms;

namespace CxbxDebugger
{
    class DebugOutputManager
    {
        List<string> DebugInfo = new List<string>();
        ListBox Handler;

        public DebugOutputManager(ListBox Target)
        {
            Handler = Target;
        }

        private void RefreshUI()
        {
            // TODO: filter
        }

        public void AddLine(string Message)
        {
            DebugInfo.Add(Message);

            if ( Handler.InvokeRequired)
            {
                Handler.Invoke(new MethodInvoker(delegate ()
                {
                    Handler.Items.Insert(0, Message);
                }));
            }
            else
            {
                Handler.Items.Insert(0, Message);
            }
        }
    }
}
