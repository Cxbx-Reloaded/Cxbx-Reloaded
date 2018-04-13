namespace CxbxDebugger
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.lbConsole = new System.Windows.Forms.ListBox();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.btnStart = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.btnSuspend = new System.Windows.Forms.ToolStripButton();
            this.btnResume = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.cbThreads = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripLabel2 = new System.Windows.Forms.ToolStripLabel();
            this.cbFrames = new System.Windows.Forms.ToolStripComboBox();
            this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.statusBar = new System.Windows.Forms.StatusStrip();
            this.lblStatus = new System.Windows.Forms.ToolStripStatusLabel();
            this.tabContainer = new System.Windows.Forms.TabControl();
            this.tabDisassembly = new System.Windows.Forms.TabPage();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.btnNext = new System.Windows.Forms.Button();
            this.btnPrev = new System.Windows.Forms.Button();
            this.btnGo = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.cbDisAddr = new System.Windows.Forms.ComboBox();
            this.tabBreakpoints = new System.Windows.Forms.TabPage();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cbAction = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.tbFilter = new System.Windows.Forms.TextBox();
            this.clbBreakpoints = new System.Windows.Forms.CheckedListBox();
            this.tabWatch = new System.Windows.Forms.TabPage();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.lvFileDetails = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.lbOpenedFiles = new System.Windows.Forms.ListBox();
            this.tabMemory = new System.Windows.Forms.TabPage();
            this.splitContainer4 = new System.Windows.Forms.SplitContainer();
            this.txMemoryDump = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnDumpMemory = new System.Windows.Forms.Button();
            this.txAddress = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.btnReadMemory = new System.Windows.Forms.Button();
            this.txSize = new System.Windows.Forms.TextBox();
            this.tabOutput = new System.Windows.Forms.TabPage();
            this.splitContainer5 = new System.Windows.Forms.SplitContainer();
            this.txFilter = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.lbDebug = new System.Windows.Forms.ListBox();
            this.diagSaveMemory = new System.Windows.Forms.SaveFileDialog();
            this.tabTweaks = new System.Windows.Forms.TabPage();
            this.button2 = new System.Windows.Forms.Button();
            this.diagBrowseCT = new System.Windows.Forms.OpenFileDialog();
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader6 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.listView2 = new System.Windows.Forms.ListView();
            this.columnHeader10 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader11 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader7 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader8 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.txDisassembly = new CxbxDebugger.RicherTextBox();
            this.toolStrip1.SuspendLayout();
            this.tableLayoutPanel3.SuspendLayout();
            this.statusBar.SuspendLayout();
            this.tabContainer.SuspendLayout();
            this.tabDisassembly.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.tabBreakpoints.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabWatch.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.tabMemory.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).BeginInit();
            this.splitContainer4.Panel1.SuspendLayout();
            this.splitContainer4.Panel2.SuspendLayout();
            this.splitContainer4.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.tabOutput.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer5)).BeginInit();
            this.splitContainer5.Panel1.SuspendLayout();
            this.splitContainer5.Panel2.SuspendLayout();
            this.splitContainer5.SuspendLayout();
            this.tabTweaks.SuspendLayout();
            this.SuspendLayout();
            // 
            // lbConsole
            // 
            this.lbConsole.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lbConsole.FormattingEnabled = true;
            this.lbConsole.Location = new System.Drawing.Point(3, 228);
            this.lbConsole.Name = "lbConsole";
            this.lbConsole.ScrollAlwaysVisible = true;
            this.lbConsole.Size = new System.Drawing.Size(756, 50);
            this.lbConsole.TabIndex = 2;
            // 
            // toolStrip1
            // 
            this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnStart,
            this.toolStripSeparator1,
            this.btnSuspend,
            this.btnResume,
            this.toolStripSeparator2,
            this.toolStripLabel1,
            this.cbThreads,
            this.toolStripLabel2,
            this.cbFrames});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional;
            this.toolStrip1.Size = new System.Drawing.Size(762, 25);
            this.toolStrip1.TabIndex = 7;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // btnStart
            // 
            this.btnStart.Image = global::CxbxDebugger.Properties.Resources.run;
            this.btnStart.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(51, 22);
            this.btnStart.Text = "Start";
            this.btnStart.Click += new System.EventHandler(this.toolStripButton1_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // btnSuspend
            // 
            this.btnSuspend.Image = global::CxbxDebugger.Properties.Resources.pause;
            this.btnSuspend.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSuspend.Name = "btnSuspend";
            this.btnSuspend.Size = new System.Drawing.Size(72, 22);
            this.btnSuspend.Text = "Suspend";
            this.btnSuspend.Click += new System.EventHandler(this.toolStripButton2_Click);
            // 
            // btnResume
            // 
            this.btnResume.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnResume.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnResume.Name = "btnResume";
            this.btnResume.Size = new System.Drawing.Size(53, 22);
            this.btnResume.Text = "Resume";
            this.btnResume.Click += new System.EventHandler(this.toolStripButton3_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(47, 22);
            this.toolStripLabel1.Text = "Thread:";
            // 
            // cbThreads
            // 
            this.cbThreads.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbThreads.Name = "cbThreads";
            this.cbThreads.Size = new System.Drawing.Size(200, 25);
            this.cbThreads.SelectedIndexChanged += new System.EventHandler(this.cbThreads_SelectedIndexChanged);
            // 
            // toolStripLabel2
            // 
            this.toolStripLabel2.Name = "toolStripLabel2";
            this.toolStripLabel2.Size = new System.Drawing.Size(74, 22);
            this.toolStripLabel2.Text = "Stack Frame:";
            // 
            // cbFrames
            // 
            this.cbFrames.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbFrames.Name = "cbFrames";
            this.cbFrames.Size = new System.Drawing.Size(200, 25);
            this.cbFrames.SelectedIndexChanged += new System.EventHandler(this.cbFrames_SelectedIndexChanged);
            // 
            // tableLayoutPanel3
            // 
            this.tableLayoutPanel3.ColumnCount = 1;
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel3.Controls.Add(this.statusBar, 0, 2);
            this.tableLayoutPanel3.Controls.Add(this.tabContainer, 0, 0);
            this.tableLayoutPanel3.Controls.Add(this.lbConsole, 0, 1);
            this.tableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel3.Location = new System.Drawing.Point(0, 25);
            this.tableLayoutPanel3.Name = "tableLayoutPanel3";
            this.tableLayoutPanel3.RowCount = 3;
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 80F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel3.Size = new System.Drawing.Size(762, 302);
            this.tableLayoutPanel3.TabIndex = 9;
            // 
            // statusBar
            // 
            this.statusBar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.statusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblStatus});
            this.statusBar.Location = new System.Drawing.Point(0, 281);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(762, 21);
            this.statusBar.TabIndex = 10;
            this.statusBar.Text = "statusStrip1";
            // 
            // lblStatus
            // 
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(39, 16);
            this.lblStatus.Text = "Ready";
            // 
            // tabContainer
            // 
            this.tabContainer.Controls.Add(this.tabDisassembly);
            this.tabContainer.Controls.Add(this.tabBreakpoints);
            this.tabContainer.Controls.Add(this.tabWatch);
            this.tabContainer.Controls.Add(this.tabMemory);
            this.tabContainer.Controls.Add(this.tabOutput);
            this.tabContainer.Controls.Add(this.tabTweaks);
            this.tabContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabContainer.Location = new System.Drawing.Point(3, 3);
            this.tabContainer.Multiline = true;
            this.tabContainer.Name = "tabContainer";
            this.tabContainer.SelectedIndex = 0;
            this.tabContainer.Size = new System.Drawing.Size(756, 219);
            this.tabContainer.TabIndex = 3;
            // 
            // tabDisassembly
            // 
            this.tabDisassembly.Controls.Add(this.splitContainer2);
            this.tabDisassembly.Location = new System.Drawing.Point(4, 22);
            this.tabDisassembly.Name = "tabDisassembly";
            this.tabDisassembly.Padding = new System.Windows.Forms.Padding(3);
            this.tabDisassembly.Size = new System.Drawing.Size(748, 193);
            this.tabDisassembly.TabIndex = 0;
            this.tabDisassembly.Text = "Disassembly";
            this.tabDisassembly.UseVisualStyleBackColor = true;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer2.IsSplitterFixed = true;
            this.splitContainer2.Location = new System.Drawing.Point(3, 3);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.btnNext);
            this.splitContainer2.Panel1.Controls.Add(this.btnPrev);
            this.splitContainer2.Panel1.Controls.Add(this.btnGo);
            this.splitContainer2.Panel1.Controls.Add(this.label6);
            this.splitContainer2.Panel1.Controls.Add(this.cbDisAddr);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.txDisassembly);
            this.splitContainer2.Size = new System.Drawing.Size(742, 187);
            this.splitContainer2.SplitterDistance = 34;
            this.splitContainer2.TabIndex = 2;
            // 
            // btnNext
            // 
            this.btnNext.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnNext.Location = new System.Drawing.Point(690, 3);
            this.btnNext.Name = "btnNext";
            this.btnNext.Size = new System.Drawing.Size(49, 23);
            this.btnNext.TabIndex = 3;
            this.btnNext.Text = "Next";
            this.btnNext.UseVisualStyleBackColor = true;
            this.btnNext.Click += new System.EventHandler(this.button4_Click);
            // 
            // btnPrev
            // 
            this.btnPrev.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnPrev.Location = new System.Drawing.Point(620, 3);
            this.btnPrev.Name = "btnPrev";
            this.btnPrev.Size = new System.Drawing.Size(64, 23);
            this.btnPrev.TabIndex = 2;
            this.btnPrev.Text = "Previous";
            this.btnPrev.UseVisualStyleBackColor = true;
            this.btnPrev.Click += new System.EventHandler(this.button3_Click);
            // 
            // btnGo
            // 
            this.btnGo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnGo.Location = new System.Drawing.Point(539, 3);
            this.btnGo.Name = "btnGo";
            this.btnGo.Size = new System.Drawing.Size(75, 23);
            this.btnGo.TabIndex = 1;
            this.btnGo.Text = "Go";
            this.btnGo.UseVisualStyleBackColor = true;
            this.btnGo.Click += new System.EventHandler(this.button2_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(3, 8);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(123, 13);
            this.label6.TabIndex = 2;
            this.label6.Text = "Disassemble by address:";
            // 
            // cbDisAddr
            // 
            this.cbDisAddr.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbDisAddr.FormattingEnabled = true;
            this.cbDisAddr.Location = new System.Drawing.Point(132, 5);
            this.cbDisAddr.Name = "cbDisAddr";
            this.cbDisAddr.Size = new System.Drawing.Size(401, 21);
            this.cbDisAddr.TabIndex = 0;
            this.cbDisAddr.SelectedIndexChanged += new System.EventHandler(this.cbDisAddr_SelectedIndexChanged);
            this.cbDisAddr.KeyDown += new System.Windows.Forms.KeyEventHandler(this.comboBox1_KeyDown);
            // 
            // tabBreakpoints
            // 
            this.tabBreakpoints.Controls.Add(this.splitContainer3);
            this.tabBreakpoints.Location = new System.Drawing.Point(4, 22);
            this.tabBreakpoints.Name = "tabBreakpoints";
            this.tabBreakpoints.Padding = new System.Windows.Forms.Padding(3);
            this.tabBreakpoints.Size = new System.Drawing.Size(748, 193);
            this.tabBreakpoints.TabIndex = 1;
            this.tabBreakpoints.Text = "Breakpoints";
            this.tabBreakpoints.UseVisualStyleBackColor = true;
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(3, 3);
            this.splitContainer3.Name = "splitContainer3";
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.groupBox4);
            this.splitContainer3.Panel1.Controls.Add(this.groupBox1);
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.clbBreakpoints);
            this.splitContainer3.Size = new System.Drawing.Size(742, 187);
            this.splitContainer3.SplitterDistance = 247;
            this.splitContainer3.TabIndex = 5;
            // 
            // groupBox4
            // 
            this.groupBox4.Location = new System.Drawing.Point(2, 112);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(241, 59);
            this.groupBox4.TabIndex = 5;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Memory";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.cbAction);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.button1);
            this.groupBox1.Controls.Add(this.tbFilter);
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(247, 106);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "File Watch";
            // 
            // cbAction
            // 
            this.cbAction.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbAction.FormattingEnabled = true;
            this.cbAction.Location = new System.Drawing.Point(95, 45);
            this.cbAction.Name = "cbAction";
            this.cbAction.Size = new System.Drawing.Size(146, 21);
            this.cbAction.TabIndex = 6;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 48);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(37, 13);
            this.label5.TabIndex = 5;
            this.label5.Text = "Action";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 22);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "Filter";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(95, 72);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(146, 23);
            this.button1.TabIndex = 4;
            this.button1.Text = "Add";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_2);
            // 
            // tbFilter
            // 
            this.tbFilter.Location = new System.Drawing.Point(95, 19);
            this.tbFilter.Name = "tbFilter";
            this.tbFilter.Size = new System.Drawing.Size(146, 20);
            this.tbFilter.TabIndex = 2;
            // 
            // clbBreakpoints
            // 
            this.clbBreakpoints.Dock = System.Windows.Forms.DockStyle.Fill;
            this.clbBreakpoints.FormattingEnabled = true;
            this.clbBreakpoints.Location = new System.Drawing.Point(0, 0);
            this.clbBreakpoints.Name = "clbBreakpoints";
            this.clbBreakpoints.Size = new System.Drawing.Size(491, 187);
            this.clbBreakpoints.TabIndex = 0;
            this.clbBreakpoints.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.clbBreakpoints_ItemCheck);
            this.clbBreakpoints.KeyDown += new System.Windows.Forms.KeyEventHandler(this.clbBreakpoints_KeyDown);
            // 
            // tabWatch
            // 
            this.tabWatch.Controls.Add(this.splitContainer1);
            this.tabWatch.Location = new System.Drawing.Point(4, 22);
            this.tabWatch.Name = "tabWatch";
            this.tabWatch.Padding = new System.Windows.Forms.Padding(3);
            this.tabWatch.Size = new System.Drawing.Size(748, 193);
            this.tabWatch.TabIndex = 2;
            this.tabWatch.Text = "File Watch";
            this.tabWatch.UseVisualStyleBackColor = true;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(3, 3);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.lvFileDetails);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.lbOpenedFiles);
            this.splitContainer1.Size = new System.Drawing.Size(742, 187);
            this.splitContainer1.SplitterDistance = 522;
            this.splitContainer1.TabIndex = 3;
            // 
            // lvFileDetails
            // 
            this.lvFileDetails.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
            this.lvFileDetails.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lvFileDetails.FullRowSelect = true;
            this.lvFileDetails.HideSelection = false;
            this.lvFileDetails.Location = new System.Drawing.Point(0, 0);
            this.lvFileDetails.Name = "lvFileDetails";
            this.lvFileDetails.Size = new System.Drawing.Size(522, 187);
            this.lvFileDetails.TabIndex = 2;
            this.lvFileDetails.UseCompatibleStateImageBehavior = false;
            this.lvFileDetails.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "State";
            this.columnHeader1.Width = 100;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Resource";
            this.columnHeader2.Width = 400;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Description";
            this.columnHeader3.Width = 200;
            // 
            // lbOpenedFiles
            // 
            this.lbOpenedFiles.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lbOpenedFiles.FormattingEnabled = true;
            this.lbOpenedFiles.Location = new System.Drawing.Point(0, 0);
            this.lbOpenedFiles.Name = "lbOpenedFiles";
            this.lbOpenedFiles.Size = new System.Drawing.Size(216, 187);
            this.lbOpenedFiles.TabIndex = 0;
            // 
            // tabMemory
            // 
            this.tabMemory.Controls.Add(this.splitContainer4);
            this.tabMemory.Location = new System.Drawing.Point(4, 22);
            this.tabMemory.Name = "tabMemory";
            this.tabMemory.Padding = new System.Windows.Forms.Padding(3);
            this.tabMemory.Size = new System.Drawing.Size(748, 193);
            this.tabMemory.TabIndex = 4;
            this.tabMemory.Text = "Memory";
            this.tabMemory.UseVisualStyleBackColor = true;
            // 
            // splitContainer4
            // 
            this.splitContainer4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer4.Location = new System.Drawing.Point(3, 3);
            this.splitContainer4.Name = "splitContainer4";
            // 
            // splitContainer4.Panel1
            // 
            this.splitContainer4.Panel1.Controls.Add(this.txMemoryDump);
            // 
            // splitContainer4.Panel2
            // 
            this.splitContainer4.Panel2.Controls.Add(this.groupBox2);
            this.splitContainer4.Size = new System.Drawing.Size(742, 187);
            this.splitContainer4.SplitterDistance = 391;
            this.splitContainer4.TabIndex = 7;
            // 
            // txMemoryDump
            // 
            this.txMemoryDump.BackColor = System.Drawing.SystemColors.Window;
            this.txMemoryDump.Cursor = System.Windows.Forms.Cursors.Default;
            this.txMemoryDump.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txMemoryDump.Font = new System.Drawing.Font("Lucida Console", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txMemoryDump.Location = new System.Drawing.Point(0, 0);
            this.txMemoryDump.Multiline = true;
            this.txMemoryDump.Name = "txMemoryDump";
            this.txMemoryDump.ReadOnly = true;
            this.txMemoryDump.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txMemoryDump.Size = new System.Drawing.Size(391, 187);
            this.txMemoryDump.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.btnDumpMemory);
            this.groupBox2.Controls.Add(this.txAddress);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.btnReadMemory);
            this.groupBox2.Controls.Add(this.txSize);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox2.Location = new System.Drawing.Point(0, 0);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(347, 187);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "View or Dump Memory";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(45, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Address";
            // 
            // btnDumpMemory
            // 
            this.btnDumpMemory.Location = new System.Drawing.Point(88, 101);
            this.btnDumpMemory.Name = "btnDumpMemory";
            this.btnDumpMemory.Size = new System.Drawing.Size(204, 23);
            this.btnDumpMemory.TabIndex = 6;
            this.btnDumpMemory.Text = "Dump Memory (to File)";
            this.btnDumpMemory.UseVisualStyleBackColor = true;
            this.btnDumpMemory.Click += new System.EventHandler(this.btnDumpMemory_Click);
            // 
            // txAddress
            // 
            this.txAddress.Location = new System.Drawing.Point(88, 19);
            this.txAddress.Name = "txAddress";
            this.txAddress.Size = new System.Drawing.Size(204, 20);
            this.txAddress.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 49);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(61, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Size (bytes)";
            // 
            // btnReadMemory
            // 
            this.btnReadMemory.Location = new System.Drawing.Point(88, 72);
            this.btnReadMemory.Name = "btnReadMemory";
            this.btnReadMemory.Size = new System.Drawing.Size(204, 23);
            this.btnReadMemory.TabIndex = 2;
            this.btnReadMemory.Text = "Read Memory";
            this.btnReadMemory.UseVisualStyleBackColor = true;
            this.btnReadMemory.Click += new System.EventHandler(this.button1_Click);
            // 
            // txSize
            // 
            this.txSize.Location = new System.Drawing.Point(88, 46);
            this.txSize.Name = "txSize";
            this.txSize.Size = new System.Drawing.Size(204, 20);
            this.txSize.TabIndex = 3;
            this.txSize.Text = "256";
            // 
            // tabOutput
            // 
            this.tabOutput.Controls.Add(this.splitContainer5);
            this.tabOutput.Location = new System.Drawing.Point(4, 22);
            this.tabOutput.Name = "tabOutput";
            this.tabOutput.Padding = new System.Windows.Forms.Padding(3);
            this.tabOutput.Size = new System.Drawing.Size(748, 193);
            this.tabOutput.TabIndex = 3;
            this.tabOutput.Text = "Debug Output";
            this.tabOutput.UseVisualStyleBackColor = true;
            // 
            // splitContainer5
            // 
            this.splitContainer5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer5.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer5.IsSplitterFixed = true;
            this.splitContainer5.Location = new System.Drawing.Point(3, 3);
            this.splitContainer5.Name = "splitContainer5";
            this.splitContainer5.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer5.Panel1
            // 
            this.splitContainer5.Panel1.Controls.Add(this.txFilter);
            this.splitContainer5.Panel1.Controls.Add(this.label3);
            // 
            // splitContainer5.Panel2
            // 
            this.splitContainer5.Panel2.Controls.Add(this.lbDebug);
            this.splitContainer5.Size = new System.Drawing.Size(742, 187);
            this.splitContainer5.SplitterDistance = 26;
            this.splitContainer5.TabIndex = 6;
            // 
            // txFilter
            // 
            this.txFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txFilter.Enabled = false;
            this.txFilter.Location = new System.Drawing.Point(41, 3);
            this.txFilter.Name = "txFilter";
            this.txFilter.Size = new System.Drawing.Size(698, 20);
            this.txFilter.TabIndex = 4;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Enabled = false;
            this.label3.Location = new System.Drawing.Point(3, 6);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(32, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Filter:";
            // 
            // lbDebug
            // 
            this.lbDebug.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lbDebug.FormattingEnabled = true;
            this.lbDebug.Location = new System.Drawing.Point(0, 0);
            this.lbDebug.Name = "lbDebug";
            this.lbDebug.ScrollAlwaysVisible = true;
            this.lbDebug.Size = new System.Drawing.Size(742, 157);
            this.lbDebug.TabIndex = 3;
            // 
            // diagSaveMemory
            // 
            this.diagSaveMemory.Filter = "Any Type|*.*";
            // 
            // tabTweaks
            // 
            this.tabTweaks.Controls.Add(this.listView2);
            this.tabTweaks.Controls.Add(this.checkBox1);
            this.tabTweaks.Controls.Add(this.listView1);
            this.tabTweaks.Controls.Add(this.button2);
            this.tabTweaks.Location = new System.Drawing.Point(4, 22);
            this.tabTweaks.Name = "tabTweaks";
            this.tabTweaks.Padding = new System.Windows.Forms.Padding(3);
            this.tabTweaks.Size = new System.Drawing.Size(748, 193);
            this.tabTweaks.TabIndex = 5;
            this.tabTweaks.Text = "Cheat Engine";
            this.tabTweaks.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(6, 6);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(132, 23);
            this.button2.TabIndex = 0;
            this.button2.Text = "Load .CT";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click_1);
            // 
            // diagBrowseCT
            // 
            this.diagBrowseCT.FileName = "openFileDialog1";
            // 
            // listView1
            // 
            this.listView1.CheckBoxes = true;
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader8,
            this.columnHeader4,
            this.columnHeader7,
            this.columnHeader6});
            this.listView1.FullRowSelect = true;
            this.listView1.Location = new System.Drawing.Point(144, 9);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(349, 181);
            this.listView1.TabIndex = 1;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "Description";
            this.columnHeader4.Width = 150;
            // 
            // columnHeader6
            // 
            this.columnHeader6.Text = "Type";
            this.columnHeader6.Width = 80;
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(6, 35);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(102, 17);
            this.checkBox1.TabIndex = 3;
            this.checkBox1.Text = "Apply on launch";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // listView2
            // 
            this.listView2.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader10,
            this.columnHeader11});
            this.listView2.FullRowSelect = true;
            this.listView2.Location = new System.Drawing.Point(499, 9);
            this.listView2.Name = "listView2";
            this.listView2.Size = new System.Drawing.Size(243, 181);
            this.listView2.TabIndex = 4;
            this.listView2.UseCompatibleStateImageBehavior = false;
            this.listView2.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader10
            // 
            this.columnHeader10.Text = "Address";
            this.columnHeader10.Width = 200;
            // 
            // columnHeader11
            // 
            this.columnHeader11.Text = "Name";
            this.columnHeader11.Width = 200;
            // 
            // columnHeader7
            // 
            this.columnHeader7.Text = "Address";
            this.columnHeader7.Width = 150;
            // 
            // columnHeader8
            // 
            this.columnHeader8.Text = "Active";
            this.columnHeader8.Width = 50;
            // 
            // txDisassembly
            // 
            this.txDisassembly.BackColor = System.Drawing.SystemColors.Window;
            this.txDisassembly.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txDisassembly.Cursor = System.Windows.Forms.Cursors.Default;
            this.txDisassembly.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txDisassembly.Font = new System.Drawing.Font("Lucida Console", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txDisassembly.Location = new System.Drawing.Point(0, 0);
            this.txDisassembly.Name = "txDisassembly";
            this.txDisassembly.ReadOnly = true;
            this.txDisassembly.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
            this.txDisassembly.Size = new System.Drawing.Size(742, 149);
            this.txDisassembly.TabIndex = 1;
            this.txDisassembly.Text = "";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(762, 327);
            this.Controls.Add(this.tableLayoutPanel3);
            this.Controls.Add(this.toolStrip1);
            this.Name = "Form1";
            this.Text = "Cxbx-Reloaded Debugger";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.tableLayoutPanel3.ResumeLayout(false);
            this.tableLayoutPanel3.PerformLayout();
            this.statusBar.ResumeLayout(false);
            this.statusBar.PerformLayout();
            this.tabContainer.ResumeLayout(false);
            this.tabDisassembly.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel1.PerformLayout();
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.tabBreakpoints.ResumeLayout(false);
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
            this.splitContainer3.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabWatch.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.tabMemory.ResumeLayout(false);
            this.splitContainer4.Panel1.ResumeLayout(false);
            this.splitContainer4.Panel1.PerformLayout();
            this.splitContainer4.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).EndInit();
            this.splitContainer4.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.tabOutput.ResumeLayout(false);
            this.splitContainer5.Panel1.ResumeLayout(false);
            this.splitContainer5.Panel1.PerformLayout();
            this.splitContainer5.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer5)).EndInit();
            this.splitContainer5.ResumeLayout(false);
            this.tabTweaks.ResumeLayout(false);
            this.tabTweaks.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ListBox lbConsole;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btnStart;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton btnSuspend;
        private System.Windows.Forms.ToolStripButton btnResume;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripComboBox cbThreads;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
        private System.Windows.Forms.TabControl tabContainer;
        private System.Windows.Forms.TabPage tabDisassembly;
        private System.Windows.Forms.TabPage tabBreakpoints;
        private System.Windows.Forms.TabPage tabWatch;
        private System.Windows.Forms.TabPage tabOutput;
        private System.Windows.Forms.ListView lvFileDetails;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ListBox lbOpenedFiles;
        private System.Windows.Forms.TabPage tabMemory;
        private System.Windows.Forms.TextBox txMemoryDump;
        private System.Windows.Forms.Button btnReadMemory;
        private System.Windows.Forms.TextBox txAddress;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox tbFilter;
        private System.Windows.Forms.StatusStrip statusBar;
        private System.Windows.Forms.ToolStripStatusLabel lblStatus;
        private RicherTextBox txDisassembly;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txSize;
        private System.Windows.Forms.Button btnDumpMemory;
        private System.Windows.Forms.SaveFileDialog diagSaveMemory;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.SplitContainer splitContainer4;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripComboBox cbFrames;
        private System.Windows.Forms.ToolStripLabel toolStripLabel2;
        private System.Windows.Forms.SplitContainer splitContainer5;
        private System.Windows.Forms.TextBox txFilter;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ListBox lbDebug;
        private System.Windows.Forms.CheckedListBox clbBreakpoints;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ComboBox cbAction;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Button btnGo;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cbDisAddr;
        private System.Windows.Forms.Button btnNext;
        private System.Windows.Forms.Button btnPrev;
        private System.Windows.Forms.TabPage tabTweaks;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.OpenFileDialog diagBrowseCT;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.ListView listView2;
        private System.Windows.Forms.ColumnHeader columnHeader10;
        private System.Windows.Forms.ColumnHeader columnHeader11;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.ColumnHeader columnHeader8;
    }
}

