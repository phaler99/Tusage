#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>

std::wstring GetProcessName(DWORD pid) {
    std::wstring name = L"";
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
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    return std::string(buf);
}

int main() {
    DWORD lastPid = 0;
    std::time_t lastStartTime = std::time(nullptr);

    while (true) {
        HWND hwnd = GetForegroundWindow();
        DWORD pid = 0;
        if (hwnd) GetWindowThreadProcessId(hwnd, &pid);

        if (pid != lastPid) {
            if (lastPid != 0) {
                std::wstring lastApp = GetProcessName(lastPid);

                if (lastApp.find(L"explorer.exe") != std::wstring::npos) {
                    wchar_t title[256];
                    GetWindowTextW(hwnd, title, 256);
                    if (wcslen(title) == 0) {
                        lastPid = pid;
                        lastStartTime = std::time(nullptr);
                        continue;
                    }
                }

                std::time_t now = std::time(nullptr);
                int duration = static_cast<int>(now - lastStartTime);

                std::cout << "Session: " << std::string(lastApp.begin(), lastApp.end())
                          << ", Duration: " << duration << " seconds, Timestamp: " << FormatTime(lastStartTime) << std::endl;
            }

            lastPid = pid;
            lastStartTime = std::time(nullptr);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}