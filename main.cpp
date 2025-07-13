#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>

std::wstring GetProcessName(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!process) return L"";
    wchar_t buffer[MAX_PATH] = {0};
    GetModuleFileNameExW(process, NULL, buffer, MAX_PATH);
    CloseHandle(process);
    return buffer;
}

std::wstring GetWindowTitle(HWND hwnd) {
    wchar_t title[256] = {0};
    GetWindowTextW(hwnd, title, 256);
    return title;
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
    std::wstring lastApp;

    while (true) {
        HWND hwnd = GetForegroundWindow();
        if (!hwnd) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        std::wstring currentApp = GetProcessName(pid);
        std::wstring title = GetWindowTitle(hwnd);
        if (currentApp.find(L"explorer.exe") != std::wstring::npos &&
            (title.empty() || title == L"Program Manager")) {
            
            if (!lastApp.empty()) {
                std::time_t now = std::time(nullptr);
                int duration = static_cast<int>(now - lastStartTime);

                std::cout << "Session: " << std::string(lastApp.begin(), lastApp.end())
                        << ", Duration: " << duration << " seconds, Timestamp: " << FormatTime(lastStartTime) << std::endl;

                lastApp.clear();
                lastPid = 0;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        if (pid != lastPid) {
            if (!lastApp.empty()) {
                std::time_t now = std::time(nullptr);
                int duration = static_cast<int>(now - lastStartTime);

                std::cout << "Session: " << std::string(lastApp.begin(), lastApp.end())
                          << ", Duration: " << duration << " seconds, Timestamp: " << FormatTime(lastStartTime) << std::endl;
            }

            lastPid = pid;
            lastApp = currentApp;
            lastStartTime = std::time(nullptr);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}