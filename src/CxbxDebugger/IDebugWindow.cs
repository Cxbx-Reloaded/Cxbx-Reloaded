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

    public interface IDebugWindow
    {
        void StartSession();

        void SuspendSession();

        void ResumeSession();

        SessionState GetSessionState();
    }

    public interface IDebugContainerWindow
    {
        void AddDebugSession(string[] Arguments, bool StartAutomatically);

        void ReportGameTitle(string GameTitle);
    }

}
