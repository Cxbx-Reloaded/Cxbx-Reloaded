// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CxbxDebugger
{
    public partial class CxbxDebuggerMain : Form, IDebugContainerWindow
    {
        public CxbxDebuggerMain(string[] args)
        {
            InitializeComponent();

            var StartupArgs = new string[args.Length - 1];
            Array.Copy(args, 1, StartupArgs, 0, args.Length - 1);

            // Setup session without initially running the game
            AddDebugSession(StartupArgs, false);

#if FALSE
            AddDebugSession(new string[] { }, false);
#endif
        }

        private void ExitToolsStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void startDebuggingToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild is IDebugWindow)
            {
                (ActiveMdiChild as IDebugWindow).StartSession();
            }
        }

        private void suspendToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild is IDebugWindow)
            {
                (ActiveMdiChild as IDebugWindow).SuspendSession();
            }
        }

        private void resumeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild is IDebugWindow)
            {
                (ActiveMdiChild as IDebugWindow).ResumeSession();
            }
        }

        public void AddDebugSession(string[] Arguments, bool StartAutomatically)
        {
            var SessionWindow = CreateNewSessionWindow(Arguments);

            if (StartAutomatically)
            {
                SessionWindow.StartSession();
            }
        }

        public void ReportGameTitle(string GameTitle)
        {
            Text = $"cxbx-debugger ({GameTitle})";
        }

        private IDebugWindow CreateNewSessionWindow(string[] Arguments)
        {
            var childForm = new CxbxDebuggerInstance(this, Arguments);
            childForm.WindowState = FormWindowState.Maximized;
            childForm.TextChanged += (sender, e) => { OnSessionWindowRenamed(sender as Form); };
            childForm.Activated += (sender, e) => { OnSessionWindowActivated(sender as IDebugWindow); };
            childForm.Show();

            return childForm;
        }

        private void OnSessionWindowActivated(IDebugWindow child)
        {
            var state = child.GetDebugStateInfo();
            RefreshStatusText(state);
        }

        private void OnSessionWindowRenamed(Form form)
        {
            // https://stackoverflow.com/questions/1347734/mdi-window-list-not-updating-child-title-bar-texts

            ActivateMdiChild(null);
            ActivateMdiChild(form);
        }

        public void ReportStatus(IDebugWindow Window, DebugState State, string Detail)
        {
            if (Window == ActiveMdiChild)
            {
                RefreshStatusText(new DebugStateInfo() { State = State, Detail = Detail });
            }
        }

        private void RefreshStatusText(DebugStateInfo stateInfo)
        {
            var stateString = "";
            var canSuspend = false;
            var canResume = false;
            var canRun = false;

            switch (stateInfo.State)
            {
                case DebugState.Unknown:
                    stateString = "No valid Xbe was loaded. Invalid session.";
                    break;

                case DebugState.Idle:
                    stateString = "Ready";
                    canRun = true;
                    break;

                case DebugState.Suspended:
                    stateString = "Suspended";
                    canResume = true;
                    break;

                case DebugState.Running:
                    stateString = "Running";
                    canSuspend = true;
                    break;

                case DebugState.Terminated:
                    stateString = "Terminated";
                    //canRun = true; // Uncomment to allow restarted sessions
                    break;
            }

            if (!string.IsNullOrEmpty(stateInfo.Detail))
            {
                stateString += $" - {stateInfo.Detail}";
            }

            miStartDebugging.Enabled = canRun;
            miSuspend.Enabled = canSuspend;
            miResume.Enabled = canResume;
            lblStatus.Text = stateString;
        }
    }
}
