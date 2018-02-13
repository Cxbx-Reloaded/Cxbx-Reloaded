// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    // Documented codes from https://msdn.microsoft.com/en-gb/library/cc704588.aspx

    public static class NtStatus
    {
        public const uint STATUS_SUCCESS = 0;

        public const uint STATUS_WAIT_1 = 1;
        public const uint STATUS_WAIT_2 = 2;
        public const uint STATUS_WAIT_3 = 3;

        public const uint STATUS_ACCESS_VIOLATION = 0xC0000005;

        public const uint STATUS_CONTROL_C_EXIT = 0xC000013A;

        public static string PrettyPrint(uint ExitCode)
        {
            string ExitCodeString;

            switch (ExitCode)
            {
                case STATUS_SUCCESS:
                    ExitCodeString = "Finished";
                    break;

                case STATUS_WAIT_1:
                case STATUS_WAIT_2:
                case STATUS_WAIT_3:
                    ExitCodeString = "Aborted";
                    break;

                case STATUS_ACCESS_VIOLATION:
                    ExitCodeString = "Access violation";
                    break;

                case STATUS_CONTROL_C_EXIT:
                    ExitCodeString = "Debug session ended";
                    break;

                default:
                    ExitCodeString = string.Format("{0:X8}", ExitCode);
                    break;
            }

            return ExitCodeString;
        }
    }
}
