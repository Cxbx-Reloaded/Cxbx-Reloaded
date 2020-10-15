// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Windows.Forms;

namespace CxbxDebugger
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            string[] args = Environment.GetCommandLineArgs();
#if !DEBUG
            if( args.Length == 1 )
            {
                // TODO: Valid usage message
                MessageBox.Show("The debugger must be launched from Cxbx with the child process arguments", "Cxbx-Debugger", MessageBoxButtons.OK, MessageBoxIcon.Information);
                Application.Exit();
            }
            else
#endif
            {
                Application.Run(new CxbxDebuggerMain(args));
            }
        }
    }
}
