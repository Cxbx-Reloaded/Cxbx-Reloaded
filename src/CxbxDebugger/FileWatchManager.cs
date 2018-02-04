// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CxbxDebugger
{
    class FileWatchInfo
    {
        public FileEventType Type;
        public string Filter;
        public bool Enabled;
    }

    class FileWatchManager
    {
        List<FileWatchInfo> WatchInfo = new List<FileWatchInfo>();
        CheckedListBox Handler;

        public FileWatchManager(CheckedListBox Target)
        {
            Handler = Target;
        }

        private void RefreshUI()
        {
            Handler.BeginUpdate();
            Handler.Items.Clear();
            foreach (FileWatchInfo Info in WatchInfo)
            {
                Handler.Items.Add(string.Format("Filter \"{0}\" with state {1}", Info.Filter, Info.Type), Info.Enabled);
            }

            Handler.EndUpdate();
        }

        public void Add(FileEventType Type, string Filter)
        {
            FileWatchInfo watch = new FileWatchInfo();

            watch.Type = Type;
            watch.Filter = Filter;
            watch.Enabled = true;

            WatchInfo.Add(watch);

            RefreshUI();
        }

        public void SetEnabled(int Index, bool Value)
        {
            if (WatchInfo[Index].Enabled != Value)
            {
                WatchInfo[Index].Enabled = Value;
                RefreshUI();
            }
        }

        public void Delete(int Index)
        {
            WatchInfo.RemoveAt(Index);
            RefreshUI();
        }

        public bool Match(FileEvents FileEvent)
        {
            var Result = WatchInfo.Find(
                Info => Info.Enabled
                && FileEvent.Type == Info.Type
                && FileEvent.Name.Contains(Info.Filter));

            return Result != null;
        }
    }
}
