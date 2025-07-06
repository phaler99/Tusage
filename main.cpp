#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <ctime>

struct Session {
    std::wstring appName;
    std::time_t startTime;
    int duration; // seconds
};

std::wstring GetProcessName(DWORD pid) {
    std::wstring name = L"Unknown";
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process) {
        wchar_t buffer[MAX_PATH];
        if (GetModuleFileNameEx(process, NULL, buffer, MAX_PATH)) {
            name = buffer;
        }
        CloseHandle(process);
    }
    return name;
}

std::string FormatTime(std::time_t t) {
    std::tm tm;
    localtime_s(&tm, &t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    return std::string(buf);
}

int main() {
    DWORD lastPid = 0;
    std::time_t lastStartTime = std::time(nullptr);
    int duration = 0;
    std::vector<Session> sessions;

    while (true) {
        HWND hwnd = GetForegroundWindow();
        if (hwnd) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);

            if (pid != lastPid) {
                // Log previous session
                if (lastPid != 0) {
                    std::time_t now = std::time(nullptr);
                    duration = static_cast<int>(now - lastStartTime);
                    std::wstring appPath = GetProcessName(lastPid);

                    std::wcout << L"App: " << appPath << L", Duration: " << duration
                               << L"s, Start: " << std::wstring(FormatTime(lastStartTime).begin(), FormatTime(lastStartTime).end()) << std::endl;

                    sessions.push_back({appPath, lastStartTime, duration});
                }
                lastPid = pid;
                lastStartTime = std::time(nullptr);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
