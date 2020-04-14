// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Runtime.InteropServices;

namespace CxbxDebugger
{
    [Flags]
    public enum ContextFlags : uint
    {
        /// <summary>
        /// Both working for 386 and 486 CPUs
        /// </summary>
        CONTEXT_i386 = 0x00010000u,
        /// <summary>
        /// SS:SP, CS:IP, FLAGS, BP
        /// </summary>
        CONTEXT_CONTROL = (CONTEXT_i386 | 0x00000001u),
        /// <summary>
        /// AX, BX, CX, DX, SI, DI
        /// </summary>
        CONTEXT_INTEGER = (CONTEXT_i386 | 0x00000002u),
        /// <summary>
        /// DS, ES, FS, GS
        /// </summary>
        CONTEXT_SEGMENTS = (CONTEXT_i386 | 0x00000004u),
        /// <summary>
        /// 387 state
        /// </summary>
        CONTEXT_FLOATING_POINT = (CONTEXT_i386 | 0x00000008u),
        /// <summary>
        /// DB 0-3,6,7
        /// </summary>
        CONTEXT_DEBUG_REGISTERS = (CONTEXT_i386 | 0x00000010u),
        /// <summary>
        /// cpu specific extensions
        /// </summary>
        CONTEXT_EXTENDED_REGISTERS = (CONTEXT_i386 | 0x00000020u),

        CONTEXT_FULL = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS,
        CONTEXT_ALL = CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS |
                                 CONTEXT_FLOATING_POINT | CONTEXT_DEBUG_REGISTERS |
                                 CONTEXT_EXTENDED_REGISTERS
    }

    [Flags]
    public enum Extendedx86ContextFlags : uint
    {
        /// <summary>
        /// Carry Flag. Set if the last arithmetic operation carried 
        /// (addition) or borrowed (subtraction) a bit beyond the 
        /// size of the register. This is then checked when the 
        /// operation is followed with an add-with-carry or 
        /// subtract-with-borrow to deal with values too large 
        /// for just one register to contain.
        /// </summary>
        Carry = 0,
        /// <summary>
        /// Parity Flag. Set if the number of set bits in the 
        /// least significant byte is a multiple of 2.
        /// </summary>
        Parity = 2,
        /// <summary>
        /// Adjust Flag. Carry of Binary Code Decimal (BCD) numbers arithmetic operations.
        /// </summary>
        Adjust = 1u << 4,
        /// <summary>
        /// Zero Flag. Set if the result of an operation is Zero (0).
        /// </summary>
        Zero = 1u << 6,
        /// <summary>
        /// Sign Flag. Set if the result of an operation is negative.
        /// </summary>
        Sign = 1u << 7,
        /// <summary>
        ///  Trap Flag. Set if step by step debugging.
        /// </summary>
        Trap = 1u << 8,
        /// <summary>
        /// Interruption Flag. Set if interrupts are enabled.
        /// </summary>
        Interruption = 1u << 9,
        /// <summary>
        ///  Direction Flag. Stream direction. If set, string operations 
        ///  will decrement their pointer rather than incrementing it, 
        ///  reading memory backwards.
        /// </summary>
        Direction = 1u << 10,
        /// <summary>
        /// Overflow Flag. Set if signed arithmetic operations result 
        /// in a value too large for the register to contain.
        /// </summary>
        Overflow = 1u << 11,
        /// <summary>
        /// I/O Privilege Level field (2 bits). I/O Privilege Level of the current process.
        /// </summary>
        IOPL_firstBit = 1u << 12,
        /// <summary>
        /// I/O Privilege Level field (2 bits). I/O Privilege Level of the current process.
        /// </summary>
        IOPL_lastBit = 1u << 13,
        /// <summary>
        /// Nested Task flag. Controls chaining of interrupts. 
        /// Set if the current process is linked to the next process.
        /// </summary>
        NestedTask = 1u << 14,
        /// <summary>
        /// Resume Flag. Response to debug exceptions.
        /// </summary>
        Resume = 1u << 16,
        /// <summary>
        /// Virtual-8086 Mode. Set if in 8086 compatibility mode.
        /// </summary>
        Virtual8086Mode = 1u << 17,
        /// <summary>
        /// Alignment Check. Set if alignment checking of memory references is done.
        /// </summary>
        AlignmentCheck = 1u << 18,
        /// <summary>
        /// Virtual Interrupt Flag. Virtual image of IF.
        /// </summary>
        VirtualInterrupt = 1u << 19,
        /// <summary>
        /// Virtual Interrupt Pending flag. Set if an interrupt is pending.
        /// </summary>
        VirtualInterruptPending = 1u << 20,
        /// <summary>
        /// Identification Flag. Support for CPUID instruction if can be set.
        /// </summary>
        Id = 1u << 21
    }

    // todo: move away from here
    public static class Constants
    {
        /// <summary>
        /// Define the size of the 80387 save area, which is in the context frame.
        /// </summary>
        public const int SIZE_OF_80387_REGISTERS = 80;


        public const int MAXIMUM_SUPPORTED_EXTENSION = 512;
    }


    [StructLayout(LayoutKind.Sequential)]
    public struct FLOATING_SAVE_AREA
    {
        public uint ControlWord;
        public uint StatusWord;
        public uint TagWord;
        public uint ErrorOffset;
        public uint ErrorSelector;
        public uint DataOffset;
        public uint DataSelector;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.SIZE_OF_80387_REGISTERS)]
        public byte[] RegisterArea;
        public uint Cr0NpxState;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CONTEXT_x86
    {
        //
        // The flags values within this flag control the contents of
        // a CONTEXT record.
        //

        // If the context record is used as an input parameter, then
        // for each portion of the context record controlled by a flag
        // whose value is set, it is assumed that that portion of the
        // context record contains valid context. If the context record
        // is being used to modify a threads context, then only that
        // portion of the threads context will be modified.
        //

        // If the context record is used as an IN OUT parameter to capture
        // the context of a thread, then only those portions of the thread's
        // context corresponding to set flags will be returned.
        //

        // The context record is never used as an OUT only parameter.
        //
        public ContextFlags ContextFlags;

        //
        // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
        // set in ContextFlags. Note that CONTEXT_DEBUG_REGISTERS is NOT
        // included in CONTEXT_FULL.
        //
        public uint Dr0;
        public uint Dr1;
        public uint Dr2;
        public uint Dr3;
        public uint Dr6;
        public uint Dr7;

        //
        // This section is specified/returned if the
        // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
        //

        public FLOATING_SAVE_AREA FloatSave;

        //
        // This section is specified/returned if the
        // ContextFlags word contians the flag CONTEXT_SEGMENTS.
        //

        public uint SegGs;
        public uint SegFs;
        public uint SegEs;
        public uint SegDs;

        //
        // This section is specified/returned if the
        // ContextFlags word contians the flag CONTEXT_INTEGER.
        //

        public uint edi;
        public uint esi;
        public uint ebx;
        public uint edx;
        public uint ecx;
        public uint eax;

        //
        // This section is specified/returned if the
        // ContextFlags word contians the flag CONTEXT_CONTROL.
        //

        /// <summary>
        /// Base/Frame pointer
        /// </summary>
        public uint ebp;
        /// <summary>
        /// Instruction pointer
        /// </summary>
        public uint eip;
        public uint segCs; // MUST BE SANITIZED
        public Extendedx86ContextFlags eFlags; // MUST BE SANITIZED
                                               /// <summary>
                                               /// Stack pointer
                                               /// </summary>
        public uint esp;
        public uint segSs;

        //
        // This section is specified/returned if the ContextFlags word
        // contains the flag CONTEXT_EXTENDED_REGISTERS.
        // The format and contexts are processor specific
        //
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = Constants.MAXIMUM_SUPPORTED_EXTENSION)]
        public byte[] ExtendedRegisters;
    }

    /// <summary>
    /// https://msdn.microsoft.com/en-us/library/windows/desktop/aa363082(v=vs.85).aspx
    /// </summary>
    public enum ExceptionCode : uint
    {
        None = 0,
        /// <summary>
        /// The thread tried to read from or write to a virtual address for which it does not have the appropriate access.
        /// </summary>
        AccessViolation = 0xC0000005,
        /// <summary>
        /// The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.
        /// </summary>
        DataTypeMisalignment = 0x80000002,
        /// <summary>
        /// A breakpoint was encountered.
        /// </summary>
        Breakpoint = 0x80000003,
        /// <summary>
        /// A trace trap or other single-instruction mechanism signaled that one instruction has been executed.
        /// </summary>
        SingleStep = 0x80000004,
        /// <summary>
        /// The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.
        /// </summary>
        ArrayBoundsExceeded = 0xC000008C,
        /// <summary>
        /// One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.
        /// </summary>
        Float_DenormalOperand = 0xC000008D,
        /// <summary>
        /// The thread tried to divide a floating-point value by a floating-point divisor of zero.
        /// </summary>
        Float_DivideByZero = 0xC000008E,
        /// <summary>
        /// The result of a floating-point operation cannot be represented exactly as a decimal fraction.
        /// </summary>
        Float_InexactResult = 0xC000008F,
        /// <summary>
        /// This exception represents any floating-point exception not included in this enumeration.
        /// </summary>
        Float_InvalidOperation = 0xC0000090,
        /// <summary>
        /// The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.
        /// </summary>
        Float_Overflow = 0xC0000091,
        /// <summary>
        /// The stack overflowed or underflowed as the result of a floating-point operation.
        /// </summary>
        Float_StackCheck = 0xC0000092,
        /// <summary>
        /// The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.
        /// </summary>
        Float_Underflow = 0xC0000093,
        /// <summary>
        /// The thread tried to divide an integer value by an integer divisor of zero.
        /// </summary>
        Integer_DivideByZero = 0xC0000094,
        /// <summary>
        /// The result of an integer operation caused a carry out of the most significant bit of the result.
        /// </summary>
        Integer_Overflow = 0xC0000095,
        /// <summary>
        /// The thread tried to execute an instruction whose operation is not allowed in the current machine mode.
        /// </summary>
        PrivilegedInstruction = 0xC0000096,
        /// <summary>
        /// The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.
        /// </summary>
        InPageError = 0xC0000006,
        /// <summary>
        /// The thread tried to execute an invalid instruction.
        /// </summary>
        IllegalInstruction = 0xC000001D,
        /// <summary>
        /// The thread tried to continue execution after a noncontinuable exception occurred.
        /// </summary>
        NoncontinuableException = 0xC0000025,
        /// <summary>
        /// The thread used up its stack.
        /// </summary>
        StackOverflow = 0xC00000FD,
        /// <summary>
        /// An exception handler returned an invalid disposition to the exception dispatcher.
        /// Programmers using a high-level language such as C should never encounter this exception.
        /// </summary>
        InvalidDisposition = 0xC0000026,
        /// <summary>
        /// 
        /// </summary>
        GuardPageViolation = 0x80000001,
        /// <summary>
        /// 
        /// </summary>
        InvalidHandle = 0xC0000008,
        /// <summary>
        /// The DBG_CONTROL_C exception code occurs when CTRL+C is input to a console process that handles CTRL+C signals and is being debugged. This exception code is not meant to be handled by applications. It is raised only for the benefit of the debugger, and is raised only when a debugger is attached to the console process.
        /// </summary>
        CtrlC = 0x40010005,
        /// <summary>
        /// Will contain 1 parameter: the address to the thrown exception object.
        /// See druntime\src\rt\deh.d
        /// </summary>
        DigitalMarsDException = 0xE0440001,
    }

    public enum RipType : uint
    {
        /// <summary>
        /// Indicates that invalid data was passed to the function that failed. This caused the application to fail.
        /// </summary>
        SLE_ERROR = 1u,
        /// <summary>
        /// Indicates that invalid data was passed to the function, but the error probably will not cause the application to fail.
        /// </summary>
        SLE_MINORERROR = 2u,
        /// <summary>
        /// Indicates that potentially invalid data was passed to the function, but the function completed processing.
        /// </summary>
        SLE_WARNING = 3u,
        /// <summary>
        /// Indicates that only dwError was set.
        /// </summary>
        None = 0u,
    }

    [Flags]
    public enum FlashWindowFlags : uint
    {
        /// <summary>
        /// Flash both the window caption and taskbar button. This is equivalent to setting the FLASHW_CAPTION | FLASHW_TRAY flags.
        /// </summary>
        FLASHW_ALL = (FLASHW_CAPTION | FLASHW_TRAY),
        /// <summary>
        /// Flash the window caption.
        /// </summary>
        FLASHW_CAPTION = 0x00000001,
        /// <summary>
        /// Stop flashing. The system restores the window to its original state.
        /// </summary>
        FLASHW_STOP = 0,
        /// <summary>
        /// Flash continuously, until the FLASHW_STOP flag is set.
        /// </summary>
        FLASHW_TIMER = 0x00000004,
        /// <summary>
        /// Flash continuously until the window comes to the foreground.
        /// </summary>
        FLASHW_TIMERNOFG = 0x0000000C,
        /// <summary>
        /// Flash the taskbar button.
        /// </summary>
        FLASHW_TRAY = 0x00000002,
    }

    public static class NativeMethods
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool GetThreadContext(IntPtr hThread, ref CONTEXT_x86 lpContext);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool SetThreadContext(IntPtr hThread, ref CONTEXT_x86 lpContext);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern uint GetThreadId(IntPtr hThread);

        [StructLayout(LayoutKind.Sequential)]
        public struct FLASHWINFO
        {
            public uint cbSize;
            public IntPtr hwnd;
            public uint dwFlags;
            public uint uCount;
            public uint dwTimeout;
        }
        [DllImport("User32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool FlashWindowEx(ref FLASHWINFO pfwi);
    }

    public static class NativeWrappers
    {
        public static void FlashWindowTray(IntPtr Handle)
        {
            var FlashParams = new NativeMethods.FLASHWINFO();
            FlashParams.cbSize = (uint)Marshal.SizeOf(typeof(NativeMethods.FLASHWINFO));
            FlashParams.dwFlags = (uint)FlashWindowFlags.FLASHW_TRAY;
            FlashParams.hwnd = Handle;

            NativeMethods.FlashWindowEx(ref FlashParams);
        }

        public static void FlashWindowTitlebar(IntPtr Handle)
        {
            var FlashParams = new NativeMethods.FLASHWINFO();
            FlashParams.cbSize = (uint)Marshal.SizeOf(typeof(NativeMethods.FLASHWINFO));
            FlashParams.dwFlags = (uint)FlashWindowFlags.FLASHW_CAPTION;
            FlashParams.hwnd = Handle;

            NativeMethods.FlashWindowEx(ref FlashParams);
        }
    }
}
