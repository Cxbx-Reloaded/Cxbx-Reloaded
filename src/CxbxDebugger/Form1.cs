// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Windows.Forms;
using System.Threading;
using System.Collections.Generic;

namespace CxbxDebugger
{
    public partial class Form1 : Form
    {
        Thread DebuggerWorkerThread;
        Debugger DebuggerInst;
        string[] CachedArgs;

        public Form1()
        {
            InitializeComponent();
            
            string[] args = Environment.GetCommandLineArgs();

            // Arguments are expected before the Form is created
            if (args.Length < 2 ) {
                throw new Exception("Incorrect usage");
            }

            var items = new List<string>(args.Length - 1);
            for (int i = 1; i < args.Length; ++i)
            {
                items.Add(args[i]);
                //listBox1.Items.Add("Arg: " + args[i]);
            }

            CachedArgs = items.ToArray();

            // TODO: Wait for user to start this?
            StartDebugging();
        }

        private void StartDebugging()
        {
            bool Create = false;

            if (DebuggerWorkerThread == null)
            {
                // First launch
                Create = true;
            }
            else if (DebuggerWorkerThread.ThreadState == System.Threading.ThreadState.Stopped)
            {
                // Further launches
                Create = true;
            }

            if (Create)
            {
                // Create debugger instance
                DebuggerInst = new Debugger(CachedArgs);
                DebuggerInst.RegisterEventListener(new DebuggerFormEvents(this));

                // Setup new debugger thread
                DebuggerWorkerThread = new Thread(x =>
                {
                    if (DebuggerInst.Launch())
                    {
                        DebuggerInst.Run();
                    }
                });

                DebuggerWorkerThread.Name = "CxbxDebugger";
                DebuggerWorkerThread.Start();
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            StartDebugging();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (DebuggerInst != null)
            {
                DebuggerInst.Break();
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (DebuggerInst != null)
            {
                DebuggerInst.Resume();
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (DebuggerWorkerThread != null)
            {
                if (DebuggerWorkerThread.ThreadState == ThreadState.Running)
                {
                    DebuggerWorkerThread.Abort();
                }
            }

            if(DebuggerInst!= null)
            {
                DebuggerInst.Dispose();
            }
        }

        public void DebugEvent(string Message)
        {
            if( listBox1.InvokeRequired )
            {
                // Ensure we Add items on the right thread
                listBox1.Invoke(new MethodInvoker(delegate ()
                {
                    listBox1.Items.Add(Message);
                }));
            }
            else
            {
                listBox1.Items.Add(Message);
            }
        }
    }

    class DebuggerFormEvents : DebuggerEventListener
    {
        readonly Form1 frm;

        public DebuggerFormEvents(Form1 main)
        {
            frm = main;
        }

        public override void OnDebugStart()
        {
            frm.DebugEvent("Started debugging session");
        }

        public override void OnDebugEnd()
        {
            frm.DebugEvent("Ended debugging session");
        }

        public override void OnModuleLoaded(string ModuleName)
        {
            frm.DebugEvent("Loading module: " + ModuleName);
        }

        public override void OnDebugOutput(string Message)
        {
            frm.DebugEvent("Debug string: " + Message);
        }

        public override void OnCallstack(string[] Callstack)
        {
            // Placeholder display
            foreach( string str in Callstack)
            {
                frm.DebugEvent("Callstack: " + str);
            }
        }
    }
}
