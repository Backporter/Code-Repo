#include <io.h>
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include "Log.h"
Log DebugLog;
#define printf(a, ...) DebugLog.WriteMessageToLog(a, __VA_ARGS__)

int main(int argc, char *argv[])
{
	DebugLog.Open(0, "\\Logger\\Log.log");
	printf("================WINSOCK SETUP========================\n");

	char buffer[256] = { 0 };
	int ret = -2;
	bool opt = TRUE;
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;

	printf("Initialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	// Forcefully attaching socket to the port 8080
	ret = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&opt, sizeof(bool));
	if (ret == SOCKET_ERROR)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8081);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);
	new_socket = accept(s, (struct sockaddr *)&client, &c);
	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
	}

	puts("Connection accepted");

	printf("=END OF WINSOCK SETUP START OF PS4 Messages==========");

	while (true) {
		ret = recv(new_socket, buffer, 256, 0);
		if (ret > 0)
		{
			printf("\n%s\n", buffer);
		}
		else if (ret == 0)
		{
			printf("Connection closed\n");
			Sleep(5000);
			break;
		}
		else
		{
			printf("recv failed: %d\n", WSAGetLastError());
			Sleep(5000);
			break;
		}
	}
	closesocket(s);
	WSACleanup();
	return 0;
}