// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;

namespace CxbxDebugger
{   
    public class DebuggerMessages
    {
        public enum ReportType : uint
        {
            HLECACHE_FILE = 0x00deed00,
            KERNEL_PATCH = 0x00deed01,
            FILE_OPENED = 0x00deed02,
            FILE_READ = 0x00deed03,
            FILE_CLOSED = 0x00deed04,
        }

        enum StringType : uint
        {
            CHAR,
            WCHAR,
        };

        public class HLECache
        {
            public string FileName { get; set; }
        }

        public static HLECache GetHLECacheReport(DebuggerThread Context, uint[] Data)
        {
            HLECache Report = new HLECache();

            StringType Type = (StringType)Data[0];
            
            if (Type != StringType.CHAR)
                throw new Exception("GetHLECacheReport expects a string message");

            uint Length = Data[1];
            IntPtr MessagePtr = new IntPtr(Data[2]);

            Report.FileName = Context.OwningProcess.ReadString(MessagePtr, Length);

            return Report;
        }

        public class KernelPatch
        {
            public string Name { get; set; }
            public IntPtr Address { get; set; }
        }

        public static KernelPatch GetKernelPatchReport(DebuggerThread Context, uint[] Data)
        {
            KernelPatch Report = new KernelPatch();

            StringType Type = (StringType)Data[0];

            if (Type != StringType.CHAR)
                throw new Exception("GetKernelPatchReport expects a string message");

            uint Length = Data[1];
            IntPtr MessagePtr = new IntPtr(Data[2]);

            Report.Name = Context.OwningProcess.ReadString(MessagePtr, Length);
            Report.Address = new IntPtr(Data[3]);

            return Report;
        }

        public class FileOpened
        {
            public IntPtr Handle { get; set; }
            public string FileName { get; set; }
        }

        public static FileOpened GetFileOpenedReport(DebuggerThread Context, uint[] Data)
        {
            FileOpened Report = new FileOpened();

            Report.Handle = new IntPtr(Data[0]);

            StringType Type = (StringType)Data[1];

            if (Type != StringType.WCHAR)
                throw new Exception("GetFileOpenedReport expects a widestring message");

            uint Length = Data[2];
            IntPtr MessagePtr = new IntPtr(Data[3]);

            Report.FileName = Context.OwningProcess.ReadWString(MessagePtr, Length);

            return Report;
        }

        public class FileRead
        {
            public IntPtr Handle { get; set; }
            public uint Length { get; set; }
        }

        public static FileRead GetFileReadReport(DebuggerThread Context, uint[] Data)
        {
            FileRead Report = new FileRead();

            Report.Handle = new IntPtr(Data[0]);
            Report.Length = Data[1];

            return Report;
        }

        public class FileClosed
        {
            public IntPtr Handle { get; set; }
        }

        public static FileClosed GetFileClosedReport(DebuggerThread Context, uint[] Data)
        {
            FileClosed Report = new FileClosed();

            Report.Handle = new IntPtr(Data[0]);

            return Report;
        }
    }
}
