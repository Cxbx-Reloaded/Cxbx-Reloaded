// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Runtime.InteropServices;

namespace CxbxDebugger.TheadHelpers
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


    public static class NativeMethods
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool GetThreadContext(IntPtr hThread, ref CONTEXT_x86 lpContext);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool SetThreadContext(IntPtr hThread, ref CONTEXT_x86 lpContext);
    }
}
