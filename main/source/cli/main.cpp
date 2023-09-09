#include <Windows.h>
#include <string>

STARTUPINFOA g_startupInfo;
PROCESS_INFORMATION g_processInfo;
SECURITY_ATTRIBUTES g_saAttr;

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hDetectChildProcessExitThread = NULL;
HANDLE g_hReadStdOutThread = NULL;

BOOL g_bChildProcessExited = FALSE;

// Mutex
HANDLE g_hDetectChildProcessExitMutex = NULL;
HANDLE g_hReadStdOutMutex = NULL;

#define BUFSIZE 8192

void OnExitHandler();
void ErrorExit(const char *);

void ReadChildStdOutput();
void HandleChildProcessExit();

bool CreateThreads()
{
    // Create mutexes

    // g_hDetectChildProcessExitThread
    g_hDetectChildProcessExitMutex = CreateMutex(NULL, FALSE, NULL);
    g_hDetectChildProcessExitThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleChildProcessExit, NULL, 0, NULL);
    if (g_hDetectChildProcessExitThread == NULL)
    {
        ErrorExit("CreateThread g_hDetectChildProcessExitThread");
        return false;
    }

    // g_hReadStdOutThread
    g_hReadStdOutMutex = CreateMutex(NULL, FALSE, NULL);
    g_hReadStdOutThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadChildStdOutput, NULL, 0, NULL);
    if (g_hReadStdOutThread == NULL)
    {
        ErrorExit("CreateThread g_hReadStdOutThread");
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    atexit(OnExitHandler);
    // Collect commandline in a local buffer
    std::string commandLine;

    commandLine += "imhex.exe ";

    // Properly add arguments to the commandline
    for (int i = 1; i < argc; ++i)
    {
        commandLine += argv[i];
        commandLine += " ";
    }

    g_saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    g_saAttr.bInheritHandle = TRUE;
    g_saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &g_saAttr, 0))
        ErrorExit("StdoutRd CreatePipe");

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
        ErrorExit("Stdout SetHandleInformation");

    // Print full commandline to the console
    // printf("imhex.exe %s\n", commandLine.data());

    // Launch "imhex.exe" with the commandline, injecting the console streams into the new process
    ZeroMemory(&g_startupInfo, sizeof(STARTUPINFOA));
    ZeroMemory(&g_processInfo, sizeof(PROCESS_INFORMATION));
    g_startupInfo.cb = sizeof(g_startupInfo);
    g_startupInfo.dwFlags |= STARTF_USESTDHANDLES;
    g_startupInfo.hStdInput = g_hChildStd_IN_Rd;
    g_startupInfo.hStdOutput = g_hChildStd_OUT_Wr;
    g_startupInfo.hStdError = g_hChildStd_OUT_Wr;

    // Launch the process
    if (!CreateProcessA("imhex.exe", commandLine.data(), NULL, NULL, TRUE, 0, NULL, NULL, &g_startupInfo, &g_processInfo))
        ErrorExit("CreateProcess");

    CreateThreads();

    // Wait for the process to exit, check g_bChildProcessExited
    while (!g_bChildProcessExited)
    {
        Sleep(10);
    }

    TerminateThread(g_hDetectChildProcessExitThread, 0);
    TerminateThread(g_hReadStdOutThread, 0);

    return 0;
}

// Handle CTRL+C using atexit
void OnExitHandler()
{
    if (g_processInfo.hProcess != NULL)
    {
        TerminateProcess(g_processInfo.hProcess, 0);
        CloseHandle(g_processInfo.hProcess);
        CloseHandle(g_processInfo.hThread);
    }

    if (g_hChildStd_IN_Rd != NULL)
        CloseHandle(g_hChildStd_IN_Rd);

    if (g_hChildStd_IN_Wr != NULL)
        CloseHandle(g_hChildStd_IN_Wr);

    if (g_hChildStd_OUT_Rd != NULL)
        CloseHandle(g_hChildStd_OUT_Rd);

    if (g_hChildStd_OUT_Wr != NULL)
        CloseHandle(g_hChildStd_OUT_Wr);
}

void ReadChildStdOutput(void)
{
    DWORD dwRead, dwWritten;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    while (true)
    {
        bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0)
            break;
        bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
        if (!bSuccess)
            break;
    }
}

void HandleChildProcessExit()
{
    DWORD dwExitCode = 0;
    BOOL bResult = FALSE;

    while (true)
    {
        bResult = GetExitCodeProcess(g_processInfo.hProcess, &dwExitCode);
        if (!bResult || dwExitCode != STILL_ACTIVE)
        {
            g_bChildProcessExited = TRUE;
            break;
        }

        Sleep(100);
    }
}

void ErrorExit(const char *lpszFunction)

// Format a readable error message, display a message box,
// and exit from the application.
{
    const char *lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    printf("Error %s: %s\n", lpszFunction, lpMsgBuf);

    LocalFree((HLOCAL)lpMsgBuf);
    ExitProcess(1);
}