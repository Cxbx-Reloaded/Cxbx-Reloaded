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
            DEBUGGER_INIT = 0x00deed05,
            FILE_WRITE = 0x00deed06,

            OVERRIDE_EXCEPTION = 0x00ceed01,
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
            public bool Succeeded{ get; set; }
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

            Report.Succeeded = Data[4] != 0;

            return Report;
        }

        public class FileRead
        {
            public IntPtr Handle { get; set; }
            public uint Length { get; set; }
            public uint Offset { get; set; }
        }

        public static FileRead GetFileReadReport(DebuggerThread Context, uint[] Data)
        {
            FileRead Report = new FileRead();

            Report.Handle = new IntPtr(Data[0]);
            Report.Length = Data[1];
            Report.Offset = Data[2];

            return Report;
        }

        public class FileWrite
        {
            public IntPtr Handle { get; set; }
            public uint Length { get; set; }
            public uint Offset { get; set; }
        }

        public static FileWrite GetFileWriteReport(DebuggerThread Context, uint[] Data)
        {
            FileWrite Report = new FileWrite();

            Report.Handle = new IntPtr(Data[0]);
            Report.Length = Data[1];
            Report.Offset = Data[2];

            return Report;
        }

        public class FileClosed
        {
            public IntPtr Handle { get; set; }
        }

        public static FileClosed GetFileClosedReport(DebuggerThread Context, uint[] Data)
        {
            // TODO: Restructure this library
            uint InvalidHandle = (uint)VsChromium.Core.Win32.Handles.NativeMethods.INVALID_HANDLE_VALUE;
            
            // Skip invalid file handles
            if (Data[0] == InvalidHandle)
                return null;

            FileClosed Report = new FileClosed();

            Report.Handle = new IntPtr(Data[0]);

            return Report;
        }

        public class ExceptionHandledQuery
        {
            public IntPtr ReponseAddr { get; set; }
            public IntPtr ExceptionAddress { get; set; }
            public uint ExceptionCode { get; set; }
            public uint ParameterCount{ get; set; }
            public IntPtr ParameterBase{ get; set; }
        }

        public static ExceptionHandledQuery GetExceptionHandledQuery(DebuggerThread Context, uint[] Data)
        {
            ExceptionHandledQuery Query = new ExceptionHandledQuery();

            Query.ReponseAddr = new IntPtr(Data[0]);
            Query.ExceptionAddress = new IntPtr(Data[1]);
            Query.ExceptionCode = Data[2];
            Query.ParameterCount = Data[3];
            Query.ParameterBase = new IntPtr(Data[4]);

            return Query;
        }

        public class DebuggerInit
        {
            public string Title { get; set; }
            public IntPtr ScreenBuffer { get; set; }
        }

        public static DebuggerInit GetDebuggerInitReport(DebuggerThread Context, uint[] Data)
        {
            DebuggerInit Report = new DebuggerInit();

            Report.ScreenBuffer = new IntPtr(Data[0]);

            StringType Type = (StringType)Data[1];

            if (Type != StringType.CHAR)
                throw new Exception("GetDebuggerInitReport expects a string message");

            uint Length = Data[2];
            IntPtr MessagePtr = new IntPtr(Data[3]);

            Report.Title = Context.OwningProcess.ReadString(MessagePtr, Length);

            return Report;
        }
    }
}
