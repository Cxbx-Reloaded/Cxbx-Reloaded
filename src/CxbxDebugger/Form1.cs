// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Windows.Forms;
using System.Threading;
using System.Collections.Generic;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using cs_x86;

namespace CxbxDebugger
{
    public partial class Form1 : Form
    {
        Thread DebuggerWorkerThread;
        Debugger DebuggerInst;
        string[] CachedArgs;

        DebuggerFormEvents DebugEvents;

        List<DebuggerThread> DebugThreads = new List<DebuggerThread>();
        List<DebuggerModule> DebugModules = new List<DebuggerModule>();

        enum FileEventType
        {
            Opened,
            Read,
            Write,
            Closed,
            OpenedFailed,
        }

        struct FileEvents
        {
            public FileEventType Type;
            public string Name;
            public uint Length;
            public uint Offset;

            public FileEvents(FileEventType Type, string Name, uint Length = 0, uint Offset = uint.MaxValue)
            {
                this.Type = Type;
                this.Name = Name;
                this.Length = Length;
                this.Offset = Offset;
            }

            public static FileEvents Opened(string Name)
            {
                return new FileEvents(FileEventType.Opened, Name);
            }

            public static FileEvents OpenedFailed(string Name)
            {
                return new FileEvents(FileEventType.OpenedFailed, Name);
            }

            public static FileEvents Read(string Name, uint Length, uint Offset)
            {
                return new FileEvents(FileEventType.Read, Name, Length, Offset);
            }

            public static FileEvents Write(string Name, uint Length, uint Offset)
            {
                return new FileEvents(FileEventType.Write, Name, Length, Offset);
            }

            public static FileEvents Closed(string Name)
            {
                return new FileEvents(FileEventType.Closed, Name);
            }
        }

        List<FileEvents> DbgFileEvents = new List<FileEvents>();
        List<DebuggerMessages.FileOpened> FileHandles = new List<DebuggerMessages.FileOpened>();

        public Form1()
        {
            InitializeComponent();

            string[] args = Environment.GetCommandLineArgs();

            // Arguments are expected before the Form is created
            if (args.Length < 2)
            {
                throw new Exception("Incorrect usage");
            }

            var items = new List<string>(args.Length - 1);
            for (int i = 1; i < args.Length; ++i)
            {
                items.Add(args[i]);
                //listBox1.Items.Add("Arg: " + args[i]);
            }

            CachedArgs = items.ToArray();

            DebugEvents = new DebuggerFormEvents(this);

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
                DebuggerInst.RegisterEventInterfaces(DebugEvents);

                // Setup new debugger thread
                DebuggerWorkerThread = new Thread(x =>
                {
                    if (DebuggerInst.Launch())
                    {
                        DebuggerInst.RunThreaded();
                    }
                });

                DebuggerWorkerThread.Name = "CxbxDebugger";
                DebuggerWorkerThread.Start();
            }
        }

        private void PopulateThreadList(ComboBox.ObjectCollection Items, DebuggerThread FocusThread)
        {
            Items.Clear();

            foreach (DebuggerThread Thread in DebugThreads)
            {
                bool IsMainThread = (Thread.Handle == Thread.OwningProcess.MainThread.Handle);
                bool IsFocusThread = (FocusThread != null) && (Thread.Handle == FocusThread.Handle);
                string DisplayStr = "";
                string PrefixStr = "";

                // Threads with focus are marked differently
                if (IsFocusThread)
                    PrefixStr = "* ";

                // Main threads always override any existing prefix
                if (IsMainThread)
                    PrefixStr = "> ";
                
                DisplayStr = string.Format("{0}[{1}] 0x{2:X8}", PrefixStr, (uint)Thread.Handle, (uint)Thread.StartAddress);
                
                if( Thread.WasSuspended )
                {
                    DisplayStr += " (suspended)";
                }

                Items.Add(DisplayStr);
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

            if (DebuggerInst != null)
            {
                DebuggerInst.Dispose();
            }
        }

        private void DebugLog(string Message)
        {
            string MessageStamped = string.Format("[{0}] {1}", DateTime.Now.ToLongTimeString(), Message);

            if (InvokeRequired)
            {
                // Ensure we Add items on the right thread
                Invoke(new MethodInvoker(delegate ()
                {
                    lbConsole.Items.Insert(0, MessageStamped);
                }));
            }
            else
            {
                lbConsole.Items.Insert(0, MessageStamped);
            }
        }

        private bool MatchString(string Haystack, string Needle)
        {
            return Haystack.Contains(Needle);
        }

        private void DebugFileEvent(FileEvents Event)
        {
            DbgFileEvents.Add(Event);

            Invoke(new MethodInvoker(delegate ()
            {
                lvFileDetails.BeginUpdate();
                {
                    var lvi = lvFileDetails.Items.Insert(0, Event.Type.ToString());
                    lvi.SubItems.Add(Event.Name);

                    switch (Event.Type)
                    {
                        case FileEventType.Read:
                        case FileEventType.Write:
                            string text = string.Format("{0} bytes", Event.Length.ToString());

                            if (Event.Offset != uint.MaxValue)
                            {
                                text += string.Format(" from offset {0}", Event.Offset);
                            }

                            lvi.SubItems.Add(text);
                            break;

                        case FileEventType.OpenedFailed:
                            lvi.SubItems.Add("Failed to open file");
                            break;
                    }
                }
                lvFileDetails.EndUpdate();

                switch (Event.Type)
                {

                    case FileEventType.Opened:
                    case FileEventType.Closed:
                    case FileEventType.OpenedFailed:
                        {
                            lbOpenedFiles.BeginUpdate();
                            lbOpenedFiles.Items.Clear();

                            foreach (DebuggerMessages.FileOpened FOpen in FileHandles)
                            {
                                lbOpenedFiles.Items.Add(FOpen.FileName);
                            }

                            lbOpenedFiles.EndUpdate();
                        }
                        break;
                }
            }));

            // Very POC check for files hit
            if (Event.Type == FileEventType.Opened)
            {
                if (tbFileBit.Text.Length > 0)
                {
                    if (MatchString(Event.Name, tbFileBit.Text))
                    {
                        if( checkBox1.Checked )
                        {
                            if (DebuggerInst != null)
                            {
                                Suspend("file open");
                            }
                        }
                    }
                }
            }
        }

        private bool DebugAsk(string Message)
        {
            return MessageBox.Show(Message, "Cxbx Debugger", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes;
        }

        private void SetDebugProcessActive(bool Active)
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(delegate ()
                {
                    // Disable when active
                    btnStart.Enabled = !Active;

                    // Enable when active
                    btnSuspend.Enabled = Active;
                    btnResume.Enabled = Active;
                }));
            }
            else
            {
                // Disable when active
                btnStart.Enabled = !Active;

                // Enable when active
                btnSuspend.Enabled = Active;
                btnResume.Enabled = Active;
            }
        }

        private void btnClearLog_Click(object sender, EventArgs e)
        {
            lbConsole.Items.Clear();
        }

        class DebuggerFormEvents : IDebuggerGeneralEvents,
            IDebuggerProcessEvents,
            IDebuggerModuleEvents,
            IDebuggerThreadEvents,
            IDebuggerOutputEvents,
            IDebuggerExceptionEvents,
            IDebuggerFileEvents
        {
            Form1 frm;

            public DebuggerFormEvents(Form1 main)
            {
                frm = main;
            }

            private string PrettyExitCode(uint ExitCode)
            {
                string ExitCodeString;

                switch (ExitCode)
                {
                    case 0:
                        ExitCodeString = "Finished";
                        break;

                    case 1:
                        ExitCodeString = "Forced to exit";
                        break;

                    case 0xC000013A:
                        // Actual code is STATUS_CONTROL_C_EXIT, but isn't very friendly
                        ExitCodeString = "Debug session ended";
                        break;

                    default:
                        ExitCodeString = string.Format("{0:X8}", ExitCode);
                        break;
                }

                return ExitCodeString;
            }

            public void OnProcessCreate(DebuggerProcess Process)
            {
                frm.DebugModules.Add(Process);
            }


            public void OnProcessExit(DebuggerProcess Process, uint ExitCode)
            {
                int remainingThreads = Process.Threads.Count;

                frm.DebugLog(string.Format("Process exited {0} ({1})", Process.ProcessID, PrettyExitCode(ExitCode)));
                frm.DebugLog(string.Format("{0} thread(s) remain open", remainingThreads));

                frm.DebugModules.Clear();

                // Ask to close if the process was forced to exit
                if (ExitCode == 1)
                {
                    if (frm.DebugAsk("The debugger was detached from the host\n\nDo you want to close the debugger?"))
                    {
                        frm.Close();
                    }
                }
            }

            public void OnDebugStart()
            {
                frm.SetDebugProcessActive(true);
                frm.DebugLog("Started debugging session");
            }

            public void OnDebugEnd()
            {
                frm.SetDebugProcessActive(false);
                frm.DebugLog("Ended debugging session");
            }

            public void OnDebugTitle(string Title)
            {
                frm.Text = string.Format("{0} - Cxbx-Reloaded Debugger", Title);
            }

            public void OnThreadCreate(DebuggerThread Thread)
            {
                frm.DebugLog(string.Format("Thread created {0}", Thread.ThreadID));
                frm.DebugThreads.Add(Thread);
            }

            public void OnThreadExit(DebuggerThread Thread, uint ExitCode)
            {
                frm.DebugLog(string.Format("Thread exited {0} ({1})", Thread.ThreadID, PrettyExitCode(ExitCode)));
                frm.DebugThreads.Remove(Thread);
            }

            public void OnModuleLoaded(DebuggerModule Module)
            {
                frm.DebugLog(string.Format("Loaded module \"{0}\"", Module.Path));
                frm.DebugModules.Add(Module);
            }

            public void OnModuleUnloaded(DebuggerModule Module)
            {
                frm.DebugLog(string.Format("Unloaded module \"{0}\"", Module.Path));
                frm.DebugModules.Remove(Module);
            }

            public void OnDebugOutput(string Message)
            {
                frm.DebugLog(string.Format("OutputDebugString \"{0}\"", Message));
            }

            public bool OnAccessViolation(DebuggerThread Thread, uint Code, IntPtr Address)
            {
                string ProcessName = "??";

                var Module = frm.DebuggerInst.ResolveModule((uint)Address);
                if( Module != null )
                {
                    ProcessName = Path.GetFileName(Module.Path);
                }
                
                // TODO Include GetLastError string
                string ExceptionMessage = string.Format("Access violation thrown at 0x{0:X8} ({1})", (uint)Address, ProcessName);

                ExceptionMessage += string.Format("\n\nException code {0:X8}", Code);

                frm.DebugLog(ExceptionMessage);

                // Already suspended at this point, so we can rebuild the callstack list
                frm.PopulateThreadList(frm.cbThreads.Items, Thread);

                ExceptionMessage += "\n\nAttempt to ignore this and risk crashing the app?";

                return frm.DebugAsk(ExceptionMessage);
            }

            public void OnFileOpened(DebuggerMessages.FileOpened Info)
            {
                if (Info.Succeeded)
                {
                    frm.FileHandles.Add(Info);
                    frm.DebugLog(string.Format("Opened file: \"{0}\"", Info.FileName));

                    frm.DebugFileEvent(FileEvents.Opened(Info.FileName));
                }
                else
                {
                    frm.DebugLog(string.Format("Opened file FAILED: \"{0}\"", Info.FileName));

                    frm.DebugFileEvent(FileEvents.OpenedFailed(Info.FileName));
                }
            }

            public void OnFileRead(DebuggerMessages.FileRead Info)
            {
                var Found = frm.FileHandles.Find(FileInfo => FileInfo.Handle == Info.Handle);
                if (Found != null)
                {
                    frm.DebugLog(string.Format("Reading {0} byte(s) from: {1}", Info.Length, Found.FileName));
                    frm.DebugFileEvent(FileEvents.Read(Found.FileName, Info.Length, Info.Offset));
                }
            }

            public void OnFileWrite(DebuggerMessages.FileWrite Info)
            {
                var Found = frm.FileHandles.Find(FileInfo => FileInfo.Handle == Info.Handle);
                if (Found != null)
                {
                    frm.DebugLog(string.Format("Writing {0} byte(s) to: {1}", Info.Length, Found.FileName));
                    frm.DebugFileEvent(FileEvents.Write(Found.FileName, Info.Length, Info.Offset));
                }
            }

            public void OnFileClosed(DebuggerMessages.FileClosed Info)
            {
                var Found = frm.FileHandles.Find(FileInfo => FileInfo.Handle == Info.Handle);
                if (Found != null)
                {
                    frm.FileHandles.Remove(Found);

                    frm.DebugFileEvent(FileEvents.Closed(Found.FileName));

                    frm.DebugLog(string.Format("Closed file: \"{0}\"", Found.FileName));
                }
            }
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            StartDebugging();
        }

        private void Suspend(string Reason)
        {
            if (DebuggerInst != null)
            {
                DebuggerInst.Break();

                NativeWrappers.FlashWindowTray(Handle);
                PopulateThreadList(cbThreads.Items, null);
            }

            lblStatus.Text = string.Format("Suspended ({0})", Reason);
        }

        private void Resume()
        {
            if (DebuggerInst != null)
            {
                DebuggerInst.Resume();
            }

            lblStatus.Text = "Running";
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            Suspend("manually triggered");
        }

        private void toolStripButton3_Click(object sender, EventArgs e)
        {
            Resume();
        }
        
        private Bitmap DumpFramebuffer()
        {
            IntPtr BufferAddr = DebuggerInst.GetScreenBuffer();
            uint BufferSize = 640 * 480 * 4;
            
            var buff = DebugThreads[0].OwningProcess.ReadMemoryBlock(BufferAddr, BufferSize);

            // todo: convert this buffer into (RGBA from BGRA)
            var bmp = new Bitmap(640, 480, PixelFormat.Format32bppRgb);

            BitmapData bmpData = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.WriteOnly, bmp.PixelFormat);
            {
                Marshal.Copy(buff, 0, bmpData.Scan0, buff.Length);
            }
            bmp.UnlockBits(bmpData);

            return bmp;
        }

        List<IntPtr> CallstackAddress = new List<IntPtr>();

        private void btnDumpCallstack_Click(object sender, EventArgs e)
        {
            int Index = cbThreads.SelectedIndex;
            if (Index == -1)
                return;

            CallstackAddress.Clear();
            lbCallstack.Items.Clear();

            int OtherModuleCount = 0;

            var Callstack = DebugThreads[Index].CallstackCache;
            foreach (DebuggerStackFrame StackFrame in Callstack.StackFrames)
            {
                string ModuleName = "??";
                uint ModuleBase = 0;
                var Module = DebuggerInst.ResolveModule((uint)StackFrame.PC);
                if( Module != null )
                {
                    if( !Module.Core )
                    {
                        OtherModuleCount++;
                        continue;
                    }

                    ModuleName = Path.GetFileName(Module.Path);
                    ModuleBase = (uint)Module.ImageBase;
                }
                else
                {
                    OtherModuleCount++;
                    continue;
                }

                if (OtherModuleCount > 0)
                {
                    CallstackAddress.Add(IntPtr.Zero);
                    lbCallstack.Items.Add("[External Code]");
                    OtherModuleCount = 0;
                }

                uint ModuleOffset = (uint)StackFrame.PC - ModuleBase;
                string FrameString = string.Format("{0} +{1:X8} ({2:X8})", ModuleName, ModuleOffset, (uint)StackFrame.PC);

                CallstackAddress.Add(StackFrame.PC);
                lbCallstack.Items.Add(FrameString);
            }

            if (OtherModuleCount > 0)
            {
                CallstackAddress.Add(IntPtr.Zero); 
                lbCallstack.Items.Add("[External Code]");
                OtherModuleCount = 0;
            }
        }

        static private bool ReadInt(TextBox Source, ref int Out)
        {
            if (int.TryParse(Source.Text, out Out))
            {
                return true;
            }

            return false;
        }

        static private bool ReadAddress(TextBox Source, ref uint Out)
        {
            if (Source.Text.StartsWith("0x"))
            {
                Out = Convert.ToUInt32(Source.Text.Substring(2), 16);
                return true;
            }
            else
            {
                if (uint.TryParse(Source.Text, out Out))
                {
                    return true;
                }
            }

            return false;
        }

        private byte[] ReadMemory()
        {
            uint addr = 0;
            if (!ReadAddress(txAddress, ref addr))
            {
                return null;
            }

            int size = 0;
            ReadInt(txSize, ref size);

            if (size < 0)
                size = 256;

            return DebugThreads[0].OwningProcess.ReadMemoryBlock(new IntPtr(addr), (uint)size);
        }
        
        private void button1_Click(object sender, EventArgs e)
        {
            if (DebuggerInst == null)
                return;

            byte[] data = ReadMemory();
            if (data == null)
                return;

            string hexData = "";

            int i = 0;
            while (i < data.Length)
            {
                hexData += string.Format("{0:X2} ", data[i]);
                ++i;

                if (i > 0 && (i % 16) == 0)
                    hexData += "\r\n";
            }

            textBox1.Text = hexData;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            // TODO Fix the frame buffer lookup
            //pictureBox1.Image = DumpFramebuffer();
        }

        private void lbCallstack_SelectedIndexChanged(object sender, EventArgs e)
        {
            if( lbCallstack.SelectedIndex != -1 )
            {
                IntPtr ptr = CallstackAddress[lbCallstack.SelectedIndex];

                if (ptr == IntPtr.Zero)
                    return;

                // Read preceeding bytes for more context
                IntPtr OffsetAddr = new IntPtr((uint)ptr - 16);

                byte[] data = DebugThreads[0].OwningProcess.ReadMemoryBlock(OffsetAddr, 64);

                string disassembly = "";

                using (Capstone cs = Capstone.CreateEngine())
                {
                    cs.DisassembleIt(data, (ulong)OffsetAddr, delegate (CapstoneInstruction Instruction)
                    {
                        string Cursor = (Instruction.Address == (uint)ptr) ? "> " : "  ";
                        disassembly += string.Format("{0}{1:x8} {2}", Cursor, Instruction.Address, Instruction.Disassembly) + "\r\n";
                    });
                }
                
                textBox3.Text = disassembly;
            }
        }

        private void btnDumpMemory_Click(object sender, EventArgs e)
        {
            if (DebuggerInst == null)
                return;

            if(diagSaveMemory.ShowDialog() == DialogResult.OK)
            {
                byte[] data = ReadMemory();
                if (data == null)
                    return;

                var Strm = File.OpenWrite(diagSaveMemory.FileNames[0]);
                Strm.Write(data, 0, data.Length);
                Strm.Close();

                MessageBox.Show("Memory dumped!");
            }
        }
    }
}
