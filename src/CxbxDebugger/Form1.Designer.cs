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
            this.btnRestart = new System.Windows.Forms.Button();
            this.btnSuspend = new System.Windows.Forms.Button();
            this.btnResume = new System.Windows.Forms.Button();
            this.lbConsole = new System.Windows.Forms.ListBox();
            this.btnClearLog = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnRestart
            // 
            this.btnRestart.Location = new System.Drawing.Point(12, 12);
            this.btnRestart.Name = "btnRestart";
            this.btnRestart.Size = new System.Drawing.Size(63, 23);
            this.btnRestart.TabIndex = 0;
            this.btnRestart.Text = "Start";
            this.btnRestart.UseVisualStyleBackColor = true;
            this.btnRestart.Click += new System.EventHandler(this.button1_Click);
            // 
            // btnSuspend
            // 
            this.btnSuspend.Location = new System.Drawing.Point(81, 12);
            this.btnSuspend.Name = "btnSuspend";
            this.btnSuspend.Size = new System.Drawing.Size(63, 23);
            this.btnSuspend.TabIndex = 1;
            this.btnSuspend.Text = "Pause";
            this.btnSuspend.UseVisualStyleBackColor = true;
            this.btnSuspend.Click += new System.EventHandler(this.button2_Click);
            // 
            // btnResume
            // 
            this.btnResume.Location = new System.Drawing.Point(150, 12);
            this.btnResume.Name = "btnResume";
            this.btnResume.Size = new System.Drawing.Size(63, 23);
            this.btnResume.TabIndex = 1;
            this.btnResume.Text = "Resume";
            this.btnResume.UseVisualStyleBackColor = true;
            this.btnResume.Click += new System.EventHandler(this.button3_Click);
            // 
            // lbConsole
            // 
            this.lbConsole.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lbConsole.FormattingEnabled = true;
            this.lbConsole.Location = new System.Drawing.Point(12, 47);
            this.lbConsole.Name = "lbConsole";
            this.lbConsole.ScrollAlwaysVisible = true;
            this.lbConsole.Size = new System.Drawing.Size(506, 212);
            this.lbConsole.TabIndex = 2;
            // 
            // btnClearLog
            // 
            this.btnClearLog.Location = new System.Drawing.Point(455, 12);
            this.btnClearLog.Name = "btnClearLog";
            this.btnClearLog.Size = new System.Drawing.Size(63, 23);
            this.btnClearLog.TabIndex = 3;
            this.btnClearLog.Text = "Clear";
            this.btnClearLog.UseVisualStyleBackColor = true;
            this.btnClearLog.Click += new System.EventHandler(this.btnClearLog_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(530, 271);
            this.Controls.Add(this.btnClearLog);
            this.Controls.Add(this.lbConsole);
            this.Controls.Add(this.btnResume);
            this.Controls.Add(this.btnSuspend);
            this.Controls.Add(this.btnRestart);
            this.Name = "Form1";
            this.Text = "Cxbx-Reloaded Debugger";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnRestart;
        private System.Windows.Forms.Button btnSuspend;
        private System.Windows.Forms.Button btnResume;
        private System.Windows.Forms.ListBox lbConsole;
        private System.Windows.Forms.Button btnClearLog;
    }
}

