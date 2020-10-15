// Written by x1nixmzeng for the Cxbx-Reloaded project
//

namespace CxbxDebugger
{
    public enum SessionState
    {
        Inactive,
        Running,
        Ended,
    }

    public enum DebugState
    {
        Unknown,
        Idle,
        Suspended,
        Breakpoint,
        Running,
        Terminated,
    }

    public struct DebugStateInfo
    {
        public DebugState State;
        public string Detail;
    }

    public interface IDebugWindow
    {
        void StartSession();

        void SuspendSession();

        void ResumeSession();

        SessionState GetSessionState();

        DebugStateInfo GetDebugStateInfo();
    }

    public interface IDebugContainerWindow
    {
        void AddDebugSession(string[] Arguments, bool StartAutomatically);

        void ReportGameTitle(string GameTitle);

        void ReportStatus(IDebugWindow Window, DebugState Status, string Detail);
    }

}
