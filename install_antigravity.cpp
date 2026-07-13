#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cwctype>
#include <cstdlib>
#include <windows.h>
#include <tlhelp32.h>
#include <lmcons.h>

// Helper to check if two wide strings are equal case-insensitively
bool iequals(const std::wstring& a, const std::wstring& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](wchar_t c1, wchar_t c2) {
        return std::towlower(c1) == std::towlower(c2);
    });
}

// Helper to check if process name matches any of the target names (case-insensitive, optionally ignoring .exe extension)
bool isTargetProcess(const std::wstring& exeName, const std::vector<std::wstring>& targets) {
    // Strip .exe from the end of exeName if present
    std::wstring baseName = exeName;
    if (baseName.size() > 4 && iequals(baseName.substr(baseName.size() - 4), L".exe")) {
        baseName = baseName.substr(0, baseName.size() - 4);
    }
    for (const auto& target : targets) {
        if (iequals(baseName, target)) {
            return true;
        }
    }
    return false;
}

// Helper to get the current user's username
std::wstring getCurrentUsername() {
    wchar_t username[UNLEN + 1];
    DWORD size = UNLEN + 1;
    if (GetUserNameW(username, &size)) {
        return std::wstring(username);
    }
    return L"";
}

// Helper to get the username owner of a given process ID
std::wstring getProcessOwner(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!hProcess) {
        // If we can't open with PROCESS_QUERY_INFORMATION, try with PROCESS_QUERY_LIMITED_INFORMATION
        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    }
    if (!hProcess) {
        return L"";
    }

    HANDLE hToken = NULL;
    if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
        CloseHandle(hProcess);
        return L"";
    }

    DWORD dwSize = 0;
    // GetTokenInformation will fail first to retrieve required size
    GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
    if (dwSize == 0) {
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return L"";
    }

    std::vector<BYTE> tokenInfo(dwSize);
    if (!GetTokenInformation(hToken, TokenUser, tokenInfo.data(), dwSize, &dwSize)) {
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return L"";
    }

    PTOKEN_USER pTokenUser = reinterpret_cast<PTOKEN_USER>(tokenInfo.data());
    wchar_t name[UNLEN + 1];
    DWORD cchName = UNLEN + 1;
    wchar_t referencedDomain[UNLEN + 1];
    DWORD cchReferencedDomain = UNLEN + 1;
    SID_NAME_USE eUse;

    std::wstring ownerName = L"";
    if (LookupAccountSidW(NULL, pTokenUser->User.Sid, name, &cchName, referencedDomain, &cchReferencedDomain, &eUse)) {
        ownerName = name;
    }

    CloseHandle(hToken);
    CloseHandle(hProcess);
    return ownerName;
}

// Helper to terminate a process by ID
bool terminateProcess(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (!hProcess) {
        return false;
    }
    BOOL result = TerminateProcess(hProcess, 1);
    CloseHandle(hProcess);
    return result != 0;
}

// Main logic to close target processes
void closeProcesses() {
    std::vector<std::wstring> targetNames = {
        L"antigravity",
        L"google-antigravity",
        L"Antigravity",
        L"winget"
    };

    std::wstring currentUser = getCurrentUsername();
    if (currentUser.empty()) {
        // Fallback to Env Var if GetUserNameW fails
        wchar_t envUser[UNLEN + 1];
        if (GetEnvironmentVariableW(L"USERNAME", envUser, UNLEN + 1) > 0) {
            currentUser = envUser;
        }
    }

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            if (isTargetProcess(pe.szExeFile, targetNames)) {
                // Check if it belongs to current user
                std::wstring owner = getProcessOwner(pe.th32ProcessID);
                if (!currentUser.empty() && iequals(owner, currentUser)) {
                    terminateProcess(pe.th32ProcessID);
                }
            }
        } while (Process32NextW(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
}

int main() {
    std::cout << "===============================" << std::endl;
    std::cout << "Closing Antigravity processes..." << std::endl;
    std::cout << "===============================" << std::endl;

    closeProcesses();

    Sleep(3000);

    std::cout << "===============================" << std::endl;
    std::cout << "Installing Google Antigravity (user mode)..." << std::endl;
    std::cout << "===============================" << std::endl;

    // Execute winget install with exactly the same arguments as the original script
    std::wstring cmd = L"winget install --id Google.Antigravity --scope user --silent --accept-package-agreements --accept-source-agreements";
    _wsystem(cmd.c_str());

    std::cout << std::endl;
    std::cout << "Done." << std::endl;

    _wsystem(L"pause");

    return 0;
}
