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
        }

        private void ExitToolsStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void CascadeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.Cascade);
        }

        private void TileVerticalToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.TileVertical);
        }

        private void TileHorizontalToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.TileHorizontal);
        }

        private void ArrangeIconsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.ArrangeIcons);
        }

        private void startDebuggingToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (Form childForm in MdiChildren)
            {
                if (childForm is IDebugWindow)
                {
                    (childForm as IDebugWindow).StartSession();
                }
            }
        }

        private void suspendToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (Form childForm in MdiChildren)
            {
                if (childForm is IDebugWindow)
                {
                    (childForm as IDebugWindow).SuspendSession();
                }
            }
        }

        private void resumeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (Form childForm in MdiChildren)
            {
                if (childForm is IDebugWindow)
                {
                    (childForm as IDebugWindow).ResumeSession();
                }
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
            var childForm = new CxbxDebuggerInstance(Arguments);
            childForm.MdiParent = this;
            childForm.WindowState = FormWindowState.Maximized;
            childForm.Show();

            return childForm;
        }
    }
}
