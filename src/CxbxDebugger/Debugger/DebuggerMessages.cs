// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;

namespace CxbxDebugger
{
    public class DebuggerMessages
    {
        public enum ReportType : uint
        {
            HLECACHE_FILE = 0x1000,

            KERNEL_PATCH = 0x2000,

            FILE_OPENED = 0x3000,

            FILE_READ = 0x3001,
            FILE_WRITE = 0x3002,
            FILE_CLOSED = 0x3003,

            DEBUGGER_INIT = 0x400,
            DEBUGGER_NEW_TARGET = 0x401,

            OVERRIDE_EXCEPTION = 0x500,

            // Exception code from https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
            MS_VC_EXCEPTION = 0x406D1388,
        }

        enum StringType : uint
        {
            CHAR,
            WCHAR,
        };

        public class DataProcessor
        {
            uint[] SourceData;
            uint SourceIndex;

            public DataProcessor(uint[] Data)
            {
                SourceData = Data;
                SourceIndex = 0;
            }

            public uint Pop()
            {
                return SourceData[SourceIndex++];
            }
        }

        public class HLECache
        {
            public string FileName { get; set; }
        }

        public static HLECache GetHLECacheReport(DebuggerThread Context, DataProcessor Data)
        {
            HLECache Report = new HLECache();

            var Type = (StringType)Data.Pop();
            if (Type != StringType.CHAR)
                throw new Exception("GetHLECacheReport expects a string message");

            var Length = Data.Pop(); ;
            var MessagePtr = new IntPtr(Data.Pop());

            Report.FileName = Context.OwningProcess.ReadString(MessagePtr, Length);

            return Report;
        }

        public class KernelPatch
        {
            public string Name { get; set; }
            public IntPtr Address { get; set; }
        }

        public static KernelPatch GetKernelPatchReport(DebuggerThread Context, DataProcessor Data)
        {
            KernelPatch Report = new KernelPatch();

            var Type = (StringType)Data.Pop();
            if (Type != StringType.CHAR)
                throw new Exception("GetKernelPatchReport expects a string message");

            var Length = Data.Pop();
            var MessagePtr = new IntPtr(Data.Pop());

            Report.Name = Context.OwningProcess.ReadString(MessagePtr, Length);
            Report.Address = new IntPtr(Data.Pop());

            return Report;
        }

        public class FileOpened
        {
            public IntPtr Handle { get; set; }
            public string FileName { get; set; }
            public bool Succeeded { get; set; }
        }

        public static FileOpened GetFileOpenedReport(DebuggerThread Context, DataProcessor Data)
        {
            FileOpened Report = new FileOpened();

            Report.Handle = new IntPtr(Data.Pop());

            var Type = (StringType)Data.Pop();
            if (Type != StringType.WCHAR)
                throw new Exception("GetFileOpenedReport expects a widestring message");

            var Length = Data.Pop();
            var MessagePtr = new IntPtr(Data.Pop());

            Report.FileName = Context.OwningProcess.ReadWString(MessagePtr, Length);
            Report.Succeeded = Data.Pop() != 0;

            return Report;
        }

        public class FileRead
        {
            public IntPtr Handle { get; set; }
            public uint Length { get; set; }
            public uint Offset { get; set; }
        }

        public static FileRead GetFileReadReport(DebuggerThread Context, DataProcessor Data)
        {
            FileRead Report = new FileRead();

            Report.Handle = new IntPtr(Data.Pop());
            Report.Length = Data.Pop();
            Report.Offset = Data.Pop();

            return Report;
        }

        public class FileWrite
        {
            public IntPtr Handle { get; set; }
            public uint Length { get; set; }
            public uint Offset { get; set; }
        }

        public static FileWrite GetFileWriteReport(DebuggerThread Context, DataProcessor Data)
        {
            FileWrite Report = new FileWrite();

            Report.Handle = new IntPtr(Data.Pop());
            Report.Length = Data.Pop();
            Report.Offset = Data.Pop();

            return Report;
        }

        public class FileClosed
        {
            public IntPtr Handle { get; set; }
        }

        public static FileClosed GetFileClosedReport(DebuggerThread Context, DataProcessor Data)
        {
            // TODO: Restructure this library
            uint InvalidHandle = (uint)VsChromium.Core.Win32.Handles.NativeMethods.INVALID_HANDLE_VALUE;

            var Handle = Data.Pop();

            // Skip invalid file handles
            if (Handle == InvalidHandle)
                return null;

            FileClosed Report = new FileClosed();

            Report.Handle = new IntPtr(Handle);

            return Report;
        }

        public class ExceptionHandledQuery
        {
            public IntPtr ReponseAddr { get; set; }
            public uint ExceptionAddress { get; set; }
            public uint ExceptionCode { get; set; }
            public uint ParameterCount { get; set; }
            public IntPtr ParameterBase { get; set; }
        }

        public static ExceptionHandledQuery GetExceptionHandledQuery(DebuggerThread Context, DataProcessor Data)
        {
            ExceptionHandledQuery Query = new ExceptionHandledQuery();

            Query.ReponseAddr = new IntPtr(Data.Pop());
            Query.ExceptionAddress = Data.Pop();
            Query.ExceptionCode = Data.Pop();
            Query.ParameterCount = Data.Pop();
            Query.ParameterBase = new IntPtr(Data.Pop());

            return Query;
        }

        public class DebuggerInit
        {
            public string Title { get; set; }
        }

        public static DebuggerInit GetDebuggerInitReport(DebuggerThread Context, DataProcessor Data)
        {
            DebuggerInit Report = new DebuggerInit();

            StringType Type = (StringType)Data.Pop();

            if (Type != StringType.CHAR)
                throw new Exception("GetDebuggerInitReport expects a string message");

            uint Length = Data.Pop();
            IntPtr MessagePtr = new IntPtr(Data.Pop());

            Report.Title = Context.OwningProcess.ReadString(MessagePtr, Length);

            return Report;
        }

        public class DebuggerNewTarget
        {
            public string CommandLine { get; set; }
        }

        public static DebuggerNewTarget GetDebuggerNewTargetReport(DebuggerThread Context, DataProcessor Data)
        {
            var Report = new DebuggerNewTarget();

            StringType Type = (StringType)Data.Pop();

            if (Type != StringType.CHAR)
                throw new Exception("GetDebuggerInitReport expects a string message");

            uint Length = Data.Pop();
            IntPtr MessagePtr = new IntPtr(Data.Pop());

            Report.CommandLine = Context.OwningProcess.ReadString(MessagePtr, Length);

            return Report;
        }

        public class MSVCThreadName
        {
            public string Name { get; set; }
            public uint ThreadId { get; set; }
        }

        public static MSVCThreadName GetMSVCThreadName(DebuggerThread Context, DataProcessor Data)
        {
            var Type = Data.Pop();
            if (Type != 0x1000)
                return null;

            string ReportName = "";

            IntPtr MessagePtr = new IntPtr(Data.Pop());
            if (MessagePtr != IntPtr.Zero)
            {
                ReportName = Context.OwningProcess.ReadString(MessagePtr);
            }

            MSVCThreadName Report = new MSVCThreadName();

            Report.Name = ReportName;
            Report.ThreadId = Data.Pop();

            return Report;
        }
    }
}
