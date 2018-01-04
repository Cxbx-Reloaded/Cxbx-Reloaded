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

            SetDebugProcessActive(false);

            // TODO: Wait for user to start this?
            //StartDebugging();
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
            string MessageStamped = string.Format("[{0}] {1}", DateTime.Now.ToLongTimeString(), Message);

            if ( lbConsole.InvokeRequired )
            {
                // Ensure we Add items on the right thread
                lbConsole.Invoke(new MethodInvoker(delegate ()
                {
                    lbConsole.Items.Insert(0, MessageStamped);
                }));
            }
            else
            {
                lbConsole.Items.Insert(0, MessageStamped);
            }
        }

        public void SetDebugProcessActive(bool Active)
        {
            if (btnRestart.InvokeRequired)
            {
                btnRestart.Invoke(new MethodInvoker(delegate ()
                {
                    // Disable when active
                    btnRestart.Enabled = !Active;

                    // Enable when active
                    btnSuspend.Enabled = Active;
                    btnResume.Enabled = Active;
                }));
            }
            else
            {
                // Disable when active
                btnRestart.Enabled = !Active;

                // Enable when active
                btnSuspend.Enabled = Active;
                btnResume.Enabled = Active;
            }
        }

        private void btnClearLog_Click(object sender, EventArgs e)
        {
            lbConsole.Items.Clear();
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
            frm.SetDebugProcessActive(true);
            frm.DebugEvent("Started debugging session");
        }

        public override void OnDebugEnd()
        {
            frm.SetDebugProcessActive(false);
            frm.DebugEvent("Ended debugging session");
        }

        public override void OnThreadCreate(DebuggerThread Thread)
        {
            frm.DebugEvent(string.Format("Thread created {0}", Thread.ThreadID));
        }

        public override void OnThreadExit(DebuggerThread Thread)
        {
            frm.DebugEvent(string.Format("Thread exited {0}", Thread.ThreadID));
        }

        public override void OnModuleLoaded(DebuggerModule Module)
        {
            frm.DebugEvent(string.Format("Loaded module \"{0}\"", Module.Path));
        }

        public override void OnModuleUnloaded(DebuggerModule Module)
        {
            frm.DebugEvent(string.Format("Unloaded module \"{0}\"", Module.Path));
        }

        public override void OnDebugOutput(string Message)
        {
            frm.DebugEvent(string.Format("OutputDebugString \"{0}\"", Message));
        }

        // TODO Update DebuggerStackFrame to query symbols
        public override void OnCallstack(DebuggerCallstack Callstack)
        {
            foreach( DebuggerStackFrame StackFrame in Callstack.StackFrames)
            {
                var stackFrameStr = string.Format("{0:X8} --> {1:X8}", (uint)StackFrame.Base, (uint)StackFrame.PC);
                frm.DebugEvent("Callstack: " + stackFrameStr);
            }
        }
    }
}
