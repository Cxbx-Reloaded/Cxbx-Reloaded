// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Windows.Forms;
using System.Threading;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using cs_x86;

namespace CxbxDebugger
{
    public partial class Form1 : Form
    {
        Thread DebuggerWorkerThread;
        Debugger DebuggerInst;
        string[] StartupArgs;
        string CachedTitle = "";
        bool SuspendedOnBp = false;

        DebuggerFormEvents DebugEvents;

        List<DebuggerThread> DebugThreads = new List<DebuggerThread>();
        List<DebuggerModule> DebugModules = new List<DebuggerModule>();
        DebuggerProcess MainProcess = null;

        FileWatchManager fileWatchMan;
        DebugOutputManager debugStrMan;
        PatchManager patchMan;

        List<DebuggerMessages.FileOpened> FileHandles = new List<DebuggerMessages.FileOpened>();

        public Form1()
        {
            InitializeComponent();

            // TODO: Cleanup arg handling

            string[] args = Environment.GetCommandLineArgs();

            // Arguments are expected before the Form is created
            if (args.Length < 2)
            {
                throw new Exception("Incorrect usage");
            }


            StartupArgs = new string[args.Length - 1];
            Array.Copy(args, 1, StartupArgs, 0, args.Length - 1);

            DebugEvents = new DebuggerFormEvents(this);

            SetDebugProcessActive(false);

            txDisassembly.InlineLinkClicked += OnDisassemblyNavigation;

            foreach (string FileEventEnum in Enum.GetNames(typeof(FileEventType)))
            {
                cbAction.Items.Add(FileEventEnum);
            }

            cbAction.SelectedIndex = 0;

            foreach (string VariableEnum in Enum.GetNames(typeof(PatchType)))
            {
                cbDataFormat.Items.Add(VariableEnum);
            }

            cbDataFormat.SelectedIndex = 0;
            InvokePatchTypeChange();

            fileWatchMan = new FileWatchManager(clbBreakpoints);
            debugStrMan = new DebugOutputManager(lbDebug);
            patchMan = new PatchManager();
        }

        private void OnDisassemblyNavigation(object sender, InlineLinkClickedEventArgs e)
        {
            // TODO: Switch to memory region
            Console.WriteLine("Attempting to view memory at " + e.Link);
            ShowDisassemblyAt(e.Link);
        }

        private void StartDebugging()
        {
            bool Create = false;

            if (DebuggerWorkerThread == null)
            {
                // First launch
                Create = true;
            }
            else if (DebuggerWorkerThread.ThreadState == ThreadState.Stopped)
            {
                // Further launches
                Create = true;
            }

            if (Create)
            {
                // Create debugger instance
                DebuggerInst = new Debugger(StartupArgs);
                DebuggerInst.RegisterEventInterfaces(DebugEvents);
                DebuggerInst.RegisterEventInterfaces(patchMan);

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

        private void PopulateThreadList(ToolStripComboBox cbItems, DebuggerThread FocusThread)
        {
            cbItems.Items.Clear();

            uint AutoThreadId = DebugThreads[0].OwningProcess.MainThread.ThreadID;
            if (FocusThread != null)
                AutoThreadId = FocusThread.ThreadID;

            int AutoIndex = 0;

            foreach (DebuggerThread Thread in DebugThreads)
            {
                bool IsMainThread = (Thread.Handle == Thread.OwningProcess.MainThread.Handle);
                bool IsFocusThread = (FocusThread != null) && (Thread.Handle == FocusThread.Handle);
                string PrefixStr = "";

                // Threads with focus are marked differently
                if (IsFocusThread)
                    PrefixStr = "* ";

                // Main threads always override any existing prefix
                if (IsMainThread)
                    PrefixStr = "> ";

                string DisplayStr = string.Format("{0}[{1}] ", PrefixStr, (uint)Thread.Handle);

                // Resolve thread name

                if (IsMainThread)
                {
                    DisplayStr += "Main Thread";
                }
                else if (Thread.DebugName != null)
                {
                    DisplayStr += Thread.DebugName;
                }
                else
                {
                    string fn = Path.GetFileName(Thread.OwningProcess.Path);
                    DisplayStr += string.Format("{0}!{1:X8}", fn, (uint)Thread.StartAddress);
                }

                // Check if the thread is already suspended

                if (Thread.WasSuspended)
                {
                    DisplayStr += " (suspended)";
                }

                if (AutoThreadId == Thread.ThreadID)
                {
                    AutoIndex = cbItems.Items.Count;
                }

                cbItems.Items.Add(DisplayStr);
            }

            // Auto-select this thread
            cbItems.SelectedIndex = AutoIndex;
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

        private void OutputString(string Message)
        {
            debugStrMan.AddLine(Message);
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

        private void DebugFileEvent(FileEvents Event)
        {
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

                        case FileEventType.FailedOpen:
                            lvi.SubItems.Add("Failed to open file");
                            break;
                    }
                }
                lvFileDetails.EndUpdate();

                switch (Event.Type)
                {

                    case FileEventType.Opened:
                    case FileEventType.Closed:
                    case FileEventType.FailedOpen:
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

            // TODO: Move to breakpoint manager
            if (fileWatchMan.Match(Event))
            {
                if (DebuggerInst != null)
                {
                    Invoke(new MethodInvoker(delegate ()
                    {
                        Suspend("file open");
                    }));
                }
            }
        }

        private void DebugTitle(string Title)
        {
            Invoke(new MethodInvoker(delegate ()
            {
                CachedTitle = Title;

                Text = string.Format("{0} - Cxbx-Reloaded Debugger", CachedTitle);

                // This is done too late - modules are already loaded
                //LoadCheatTable(string.Format("{0}.ct", CachedTitle));
            }));
        }

        private bool DebugAsk(string Message)
        {
            return MessageBox.Show(Message, "Cxbx Debugger", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes;
        }

        private void DebugBreakpoint(DebuggerModule Module, uint Address)
        {
            Invoke(new MethodInvoker(delegate ()
            {
                SuspendedOnBp = true;

                bool auto_resume = true;
                if (cbBreakpointAll.Checked == false)
                {
                    // Ignore all submodule breakpoints
                    // (effectively triggers from Cxbx.exe and default.xbe)

                    if (cbBreakpointCxbx.Checked)
                    {
                        auto_resume = (!Module.Core);
                    }
                }

                if (auto_resume)
                {
                    Resume();
                }
                else
                {
                    string module_name = Path.GetFileName(Module.Path);
                    Suspend(string.Format("Breakpoint hit in {0} at 0x{1:x}", module_name, Address));

                    // Forces a refresh at the breakpoint address (not the callstack trace)
                    DumpDisassembly(Address);
                }

            }));
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

                    lblStatus.Text = (Active ? "Running" : "Inactive");
                }));
            }
            else
            {
                // Disable when active
                btnStart.Enabled = !Active;

                // Enable when active
                btnSuspend.Enabled = Active;
                btnResume.Enabled = Active;

                lblStatus.Text = (Active ? "Running" : "Inactive");
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

            public void OnProcessCreate(DebuggerProcess Process)
            {
                frm.DebugModules.Add(Process);
                frm.MainProcess = Process;
            }

            public void OnProcessExit(DebuggerProcess Process, uint ExitCode)
            {
                int remainingThreads = Process.Threads.Count;

                frm.DebugLog(string.Format("Process exited {0} ({1})", Process.ProcessID, NtStatus.PrettyPrint(ExitCode)));
                frm.DebugLog(string.Format("{0} child thread(s) remain open", remainingThreads));

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

            public void OnDebugTitleLoaded(string Title)
            {
                frm.DebugLog(string.Format("Loaded title \"{0}\"", Title));
                frm.DebugTitle(Title);
            }

            public void OnThreadCreate(DebuggerThread Thread)
            {
                frm.DebugLog(string.Format("Thread created {0}", Thread.ThreadID));
                frm.DebugThreads.Add(Thread);
            }

            public void OnThreadExit(DebuggerThread Thread, uint ExitCode)
            {
                frm.DebugLog(string.Format("Thread exited {0} ({1})", Thread.ThreadID, NtStatus.PrettyPrint(ExitCode)));
                frm.DebugThreads.Remove(Thread);
            }

            public void OnThreadNamed(DebuggerThread Thread)
            {
                frm.DebugLog(string.Format("Thread {0} named {1}", Thread.ThreadID, Thread.DebugName));
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
                frm.OutputString(Message);
            }

            public bool OnAccessViolation(DebuggerThread Thread, uint Code, uint Address)
            {
                string ProcessName = "??";

                var Module = frm.DebuggerInst.ResolveModule(Address);
                if (Module != null)
                {
                    ProcessName = Path.GetFileName(Module.Path);
                }

                // TODO Include GetLastError string
                string ExceptionMessage = string.Format("Access violation thrown at 0x{0:X8} ({1})", Address, ProcessName);

                ExceptionMessage += string.Format("\n\nException code {0:X8}", Code);

                frm.DebugLog(ExceptionMessage);

                // Already suspended at this point, so we can rebuild the callstack list
                frm.PopulateThreadList(frm.cbThreads, Thread);

                ExceptionMessage += "\n\nAttempt to ignore this and risk crashing the app?";

                return frm.DebugAsk(ExceptionMessage);
            }

            public void OnBreakpoint(DebuggerThread Thread, uint Address, uint Code, bool FirstChance)
            {
                frm.DebugLog(string.Format("Breakpoint hit at 0x{0:X8} with code {1:X8}", Address, Code));

                var Module = frm.DebuggerInst.ResolveModule(Address);
                if (Module != null)
                {
                    frm.DebugBreakpoint(Module, Address);
                }
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
                if (!SuspendedOnBp)
                {
                    DebuggerInst.Break();
                }

                // Update thread context cache
                DebuggerInst.Trace();

                NativeWrappers.FlashWindowTray(Handle);
                PopulateThreadList(cbThreads, null);
            }

            lblStatus.Text = string.Format("Suspended ({0})", Reason);

            cbThreads.Enabled = true;
            cbFrames.Enabled = true;
        }

        private void Resume()
        {
            if (DebuggerInst != null)
            {
                if (SuspendedOnBp)
                {
                    DebuggerInst.BreakpointContinue();
                    SuspendedOnBp = false;
                }
                else
                {
                    DebuggerInst.Resume();
                }
            }

            lblStatus.Text = "Running";

            cbThreads.Enabled = false;
            cbFrames.Enabled = false;
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            Suspend("manually triggered");
        }

        private void toolStripButton3_Click(object sender, EventArgs e)
        {
            Resume();
        }

        struct CallstackInfo
        {
            public uint InstructionPointer;
            public uint ModuleBase;

            public CallstackInfo(uint IP, uint Base)
            {
                InstructionPointer = IP;
                ModuleBase = Base;
            }
        };

        List<CallstackInfo> CallstackDump = new List<CallstackInfo>();

        static private bool ReadAddress(ComboBox Source, ref uint Out)
        {
            string SelString = Source.Text;

            if (Common.ReadHex(SelString, ref Out))
            {
                // Only add new addresses
                if (Source.SelectedIndex == -1)
                {
                    Source.Items.Insert(0, SelString);
                }
                return true;
            }

            return false;
        }

        private byte[] ReadMemory()
        {
            uint addr = 0;
            if (!Common.ReadHex(txAddress.Text, ref addr))
            {
                return null;
            }

            int size = 0;
            Common.ReadNumeric(txSize.Text, ref size);

            if (size < 0)
                size = 256;

            // Pick any thread to find the owner process
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

            txMemoryDump.Text = hexData;
        }

        public delegate void DisResultOther(string Part);
        public delegate void DisResultAddress(uint Address);

        public static void ExtractSymbols(string Text, DisResultOther ProcessOtherData, DisResultAddress ProcessAddrData, DisResultAddress ProcessIndirectAddr)
        {
            // This regex will match addresses in the format "0x123"
            // TODO: Fix ajoined addresses ie "0x1230x123" - treated as 0x1230
            MatchCollection Matches = Regex.Matches(Text, "(-?0x[a-f0-9]+)", RegexOptions.IgnoreCase);

            int LastIndex = 0;
            for (int i = 0; i < Matches.Count; ++i)
            {
                if (Matches[i].Index > LastIndex)
                {
                    var Last = Text.Substring(LastIndex, Matches[i].Index - LastIndex);
                    ProcessOtherData(Last);
                }

                string MatchStr = Matches[i].ToString();
                if (MatchStr.StartsWith("-"))
                {
                    uint Address = Convert.ToUInt32(MatchStr.Substring(1), 16);
                    Address = ~Address;
                    ProcessAddrData(Address);
                }
                else
                {
                    uint Address = Convert.ToUInt32(MatchStr, 16);
                    ProcessAddrData(Address);
                }

                LastIndex = Matches[i].Index + Matches[i].Length;
            }

            if (LastIndex < Text.Length)
            {
                ProcessOtherData(Text.Substring(LastIndex));
            }
        }

        class SymbolInfoHelper
        {
            uint EP = 0;
            string Name = "";

            public SymbolInfoHelper(Debugger dbgr, uint Address)
            {
                var Module = dbgr.ResolveModule(Address);
                if (Module != null)
                {
                    EP = (uint)Module.ImageBase;
                    Name = Path.GetFileName(Module.Path);
                }
            }

            public void GenerateLink(RicherTextBox tb, uint Address)
            {
                if (EP != 0)
                {
                    string LinkName = string.Format("{0} +{1:x}", Name, Address - EP);
                    string Link = string.Format("0x{0:x8}", Address);

                    tb.InsertLink(LinkName, Link);
                }
                else
                {
                    tb.Add(string.Format("0x{0:X8}", Address));
                }
            }
        }

        private void DumpDisassembly(uint DisAddress)
        {
            // No threads
            if (DebugThreads.Count == 0)
                return;

            // Read preceeding bytes for more context
            // TODO: This MUST align with a previous instruction or our disassembler will fail
            uint OffsetAddr = DisAddress; // -16

            byte[] data = DebugThreads[0].OwningProcess.ReadMemoryBlock(new IntPtr(OffsetAddr), 64);

            // Dump requested after crashing - and read memory handles this silently
            if (data == null)
                return;

            txDisassembly.BeginUpdate();
            txDisassembly.Clear();

            // TODO: Needs refactoring

            var ModuleInfo = new SymbolInfoHelper(DebuggerInst, OffsetAddr);

            // TODO: "call dword ptr [0x00XXXXXX]" instructions should be resolved
            using (Capstone cs = Capstone.CreateEngine())
            {
                cs.DisassembleIt(data, OffsetAddr, delegate (CapstoneInstruction Instruction)
                {
                    string Cursor = (Instruction.Address == DisAddress) ? "> " : "  ";

                    txDisassembly.Add(Cursor);
                    ModuleInfo.GenerateLink(txDisassembly, (uint)Instruction.Address);
                    txDisassembly.Add(" ");

                    ExtractSymbols
                    (
                        Instruction.Disassembly,

                        // Regular instruction text
                        delegate (string RegData)
                        {
                            txDisassembly.Add(RegData);
                        },

                        // Raw address
                        delegate (uint address)
                        {
                            var Info = new SymbolInfoHelper(DebuggerInst, address);
                            Info.GenerateLink(txDisassembly, address);
                        },

                        // Indirect address
                        delegate (uint address)
                        {
                            // stub
                        });

                    txDisassembly.AddLine("");
                });
            }

            txDisassembly.EndUpdate();
            txDisassembly.Select(0, 0);
        }

        private void cbFrames_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cbFrames.SelectedIndex != -1)
            {
                CallstackInfo info = CallstackDump[cbFrames.SelectedIndex];

                if (info.InstructionPointer == 0)
                    return;

                DumpDisassembly(info.InstructionPointer);
            }
        }

        private void btnDumpMemory_Click(object sender, EventArgs e)
        {
            if (DebuggerInst == null)
                return;

            if (diagSaveMemory.ShowDialog() == DialogResult.OK)
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

        private void DumpCallstack(bool ShowExternal = true)
        {
            int Index = cbThreads.SelectedIndex;
            if (Index == -1)
                return;

            CallstackDump.Clear();
            cbFrames.Items.Clear();

            int OtherModuleCount = 0;

            var Callstack = DebugThreads[Index].CallstackCache;
            foreach (DebuggerStackFrame StackFrame in Callstack.StackFrames)
            {
                string ModuleName = "??";
                uint ModuleBase = 0;
                var Module = DebuggerInst.ResolveModule((uint)StackFrame.PC);
                if (Module != null)
                {
                    if (!ShowExternal)
                    {
                        if (!Module.Core)
                        {
                            OtherModuleCount++;
                            continue;
                        }
                    }

                    ModuleName = Path.GetFileName(Module.Path);
                    ModuleBase = (uint)Module.ImageBase;
                }
                else
                {
                    if (!ShowExternal)
                    {
                        OtherModuleCount++;
                    }
                    continue;
                }

                if (!ShowExternal)
                {
                    if (OtherModuleCount > 0)
                    {
                        CallstackDump.Add(new CallstackInfo());
                        cbFrames.Items.Add("[External Code]");
                        OtherModuleCount = 0;
                    }
                }

                uint ModuleOffset = (uint)StackFrame.PC - ModuleBase;
                string FrameString = string.Format("{0} +{1:X8} ({2:X8})", ModuleName, ModuleOffset, (uint)StackFrame.PC);

                // To program counter
                CallstackDump.Add(new CallstackInfo((uint)StackFrame.PC, ModuleBase));
                cbFrames.Items.Add(FrameString);
            }

            if (!ShowExternal)
            {
                if (OtherModuleCount > 0)
                {
                    CallstackDump.Add(new CallstackInfo());
                    cbFrames.Items.Add("[External Code]");
                    OtherModuleCount = 0;
                }
            }

            if (cbFrames.Items.Count > 0)
            {
                // Auto-select the first item to dump
                cbFrames.SelectedIndex = 0;
            }
        }

        private void cbThreads_SelectedIndexChanged(object sender, EventArgs e)
        {
            DumpCallstack();
        }

        private void ShowMemoryAt(string Address)
        {
            // Switch to memory page and set address string
            tabContainer.SelectedTab = tabMemory;
            txAddress.Text = string.Format("0x{0}", Address);
        }

        private void ShowDisassemblyAt(string Address)
        {
            tabContainer.SelectedTab = tabDisassembly;

            uint addr = 0;
            if (Common.ReadHex(Address, ref addr))
            {
                // Insert disassembly history
                // TODO: Keep symbol name
                cbDisAddr.Items.Insert(0, Address);
                cbDisAddr.Text = Address;
                DumpDisassembly(addr);
            }
        }

        private void button1_Click_2(object sender, EventArgs e)
        {
            FileEventType Event = (FileEventType)cbAction.SelectedIndex;
            fileWatchMan.Add(Event, tbFilter.Text);
        }

        private void clbBreakpoints_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                if (clbBreakpoints.SelectedIndex != -1)
                {
                    fileWatchMan.Delete(clbBreakpoints.SelectedIndex);
                }
            }
        }

        private void clbBreakpoints_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            fileWatchMan.SetEnabled(e.Index, e.NewValue == CheckState.Checked);
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (keyData == Keys.F5)
            {
                StartDebugging();
                return true;
            }

            return false;
        }

        private void HandleDisasmGo()
        {
            uint addr = 0;
            if (ReadAddress(cbDisAddr, ref addr))
            {
                DumpDisassembly(addr);
            }
        }

        private void HandleDisasmGo(int Offset)
        {
            int TargetIndex = cbDisAddr.SelectedIndex + Offset;

            if (TargetIndex < 0)
                return;

            if (TargetIndex < cbDisAddr.Items.Count)
            {
                cbDisAddr.SelectedIndex = TargetIndex;
                HandleDisasmGo();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            HandleDisasmGo();
        }

        private void comboBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                HandleDisasmGo();
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            HandleDisasmGo(1);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            HandleDisasmGo(-1);
        }

        private void cbDisAddr_SelectedIndexChanged(object sender, EventArgs e)
        {
            HandleDisasmGo();
        }

        private void RefreshPatches()
        {
            lvCEMemory.BeginUpdate();
            lvCEMemory.Items.Clear();

            foreach (Patch DataPatch in patchMan.Data)
            {
                var li = lvCEMemory.Items.Add(string.Format("{0} 0x{1:x}", DataPatch.Module, DataPatch.Offset));
                li.SubItems.Add(DataPatch.Name);
                li.SubItems.Add(string.Format("{0} byte(s)", DataPatch.Patched.Length));
                if (MainProcess != null)
                {
                    li.SubItems.Add(patchMan.Read(MainProcess, DataPatch));
                }
                else
                {
                    li.SubItems.Add("??");
                }
            }

            lvCEMemory.EndUpdate();

            lvCEAssembly.BeginUpdate();
            lvCEAssembly.Items.Clear();

            foreach (Patch patch in patchMan.Assembly)
            {
                var li = lvCEAssembly.Items.Add(string.Format("{0} 0x{1:x}", patch.Module, patch.Offset));
                li.SubItems.Add(patch.Name);
                li.SubItems.Add(PrettyPrint(patch.Original));
                li.SubItems.Add(PrettyPrint(patch.Patched));
            }

            lvCEAssembly.EndUpdate();
        }

        static private byte[] Fill(int Count, byte Val)
        {
            List<byte> ByteList = new List<byte>(Count);
            for (int i = 0; i < Count; ++i) ByteList.Add(Val);
            return ByteList.ToArray();
        }

        static private byte[] Nops(int Count)
        {
            return Fill(Count, 0x90);
        }

        static private string PrettyPrint(byte[] Bytes)
        {
            if (Bytes == null)
                return "??";

            string Str = "";
            int Max = Math.Min(5, Bytes.Length);
            for (int i = 0; i < Max; ++i)
            {
                Str += string.Format("{0:X2} ", Bytes[i]);
            }
            return Str;
        }

        private void LoadCheatTable(string filename)
        {
            string path = Directory.GetCurrentDirectory();

            if (File.Exists(filename))
            {
                DebugLog(string.Format("Attempting to load \"{0}\"", filename));

                CheatEngine.CheatTable ct_data = CheatEngine.CheatTableReader.FromFile(filename);
                if (ct_data != null)
                {
                    foreach (CheatEngine.CheatEntry Entry in ct_data.CheatEntries)
                    {
                        uint addr = 0;
                        if (Common.ReadHex(Entry.Address, ref addr))
                        {
                            Patch DataPatch = new Patch();

                            DataPatch.DisplayAs = PatchType.Array;
                            DataPatch.Name = Entry.Description;
                            DataPatch.Module = "";
                            DataPatch.Offset = addr;
                            DataPatch.Original = null;
                            DataPatch.Patched = Nops(CheatEngine.Helpers.VariableSize(Entry.VariableType));

                            patchMan.Data.Add(DataPatch);
                        }
                    }

                    foreach (CheatEngine.CodeEntry Entry in ct_data.CodeEntires)
                    {
                        Patch DataPatch = new Patch();

                        DataPatch.DisplayAs = PatchType.Array;
                        DataPatch.Name = Entry.Description;
                        DataPatch.Module = Entry.ModuleName;
                        DataPatch.Offset = Entry.ModuleNameOffset;
                        DataPatch.Original = Entry.Actual;
                        DataPatch.Patched = Nops(Entry.Actual.Length);

                        patchMan.Assembly.Add(DataPatch);
                    }

                    DebugLog(string.Format("Loaded {0} auto-assembler entries", ct_data.CodeEntires.Count));
                    DebugLog(string.Format("Loaded {0} cheat entries", ct_data.CheatEntries.Count));

                    RefreshPatches();
                }
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            PatchType PatchType = (PatchType)cbDataFormat.SelectedIndex;

            int PatchSize = PatchManager.PatchTypeLength(PatchType);
            if (PatchSize == 0)
            {
                if (!Common.ReadNumeric(textBox2.Text, ref PatchSize))
                    return;

                if (PatchSize < 0)
                    return;
            }

            uint addr = 0;
            if (Common.ReadHex(txAddress.Text, ref addr))
            {
                Patch DataPatch = new Patch();

                DataPatch.DisplayAs = PatchType;
                DataPatch.Name = string.Format("Patched {0}", PatchType);
                DataPatch.Module = "";
                DataPatch.Offset = addr;

                // TODO: Read original memory at this location
                DataPatch.Original = Nops(PatchSize);
                DataPatch.Patched = Nops(PatchSize);
                patchMan.Data.Add(DataPatch);

                RefreshPatches();
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            uint addr = 0;
            if (ReadAddress(cbDisAddr, ref addr))
            {
                txAddress.Text = cbDisAddr.Text;
                tabContainer.SelectedTab = tabMemory;
            }
        }

        private void button2_Click_2(object sender, EventArgs e)
        {
            if (diagBrowseCT.ShowDialog() == DialogResult.OK)
            {
                string filename = diagBrowseCT.FileNames[0];
                LoadCheatTable(filename);
            }
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            RefreshPatches();
        }

        private void InvokePatchTypeChange()
        {
            PatchType Type = (PatchType)cbDataFormat.SelectedIndex;

            textBox2.Text = PatchManager.PatchTypeLength(Type).ToString();
            textBox2.Enabled = (Type == PatchType.Array);
        }

        private void cbDataFormat_SelectionChangeCommitted(object sender, EventArgs e)
        {
            InvokePatchTypeChange();
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            if (lvCEMemory.SelectedIndices.Count != 1)
                return;

            string Value = txNewValue.Text;
            if (Value.Length != 0)
            {
                Patch DataPatch = patchMan.Data[lvCEMemory.SelectedIndices[0]];
                if (patchMan.Write(MainProcess, DataPatch, Value))
                {
                    RefreshPatches();
                }
            }
        }
    }
}
