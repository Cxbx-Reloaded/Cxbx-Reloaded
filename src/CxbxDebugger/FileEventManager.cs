// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CxbxDebugger
{
    enum FileEventType
    {
        Opened,
        Read,
        Write,
        Closed,
        FailedOpen,
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
            return new FileEvents(FileEventType.FailedOpen, Name);
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
}
