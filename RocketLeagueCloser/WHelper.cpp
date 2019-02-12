#include <windows.h>
#include <Psapi.h>

#include <string>

namespace wh {

	static HWND getForegroundWindow() {
		return GetForegroundWindow();
	}

	static std::string getTitle(HWND handle) {
		int length = GetWindowTextLength(handle);
		if (!length) return "";

		TCHAR buffer[MAX_PATH] = { 0 };

		GetWindowText(handle, buffer, MAX_PATH);

		return std::string(buffer);
	}

	static std::string getExecutable(HWND handle) {
		TCHAR buffer[MAX_PATH] = { 0 };
		DWORD dwProcId = 0;

		GetWindowThreadProcessId(handle, &dwProcId);

		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);

		GetModuleFileNameEx(hProc, 0, buffer, MAX_PATH);
		CloseHandle(hProc);

		return std::string(buffer);
	}

	static BOOL isVisible(HWND handle) {
		return IsWindowVisible(handle);
	}

}
