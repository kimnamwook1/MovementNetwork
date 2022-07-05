#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")

void MyMemSet(char* p, int Value, size_t Size)
{
	for (size_t i = 0; i < Size; ++i)
	{
		p[i] = Value;
	}
}

void MyMemCopy(char* Destination, char* Source,  size_t Size)
{
	for (int i = 0; i < Size; ++i)
	{
		Destination[i] = Source[i];
	}
}

char Map[10][10] = {
	{'0','0','0','0','0','0','0','0','0','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','0','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','0','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','1','1','1','1','1','1','1','1','0'},
	{'0','0','0','0','0','0','0','0','0','0'}
};

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerAddrIn;
	memset(&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	//char* p = (char*)&ServerAddrIn;
	MyMemSet((char*)&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	ServerAddrIn.sin_family = AF_INET;
	ServerAddrIn.sin_port = htons(1234);
	ServerAddrIn.sin_addr.s_addr = INADDR_ANY;

	bind(ServerSocket, (SOCKADDR*)& ServerAddrIn, sizeof(SOCKADDR_IN));

	listen(ServerSocket, 0);

	SOCKADDR_IN ClientSockAddrIn;
	MyMemSet((char*)&ClientSockAddrIn, 0, sizeof(SOCKADDR_IN));
	int ClientSockAddrInSize = sizeof(SOCKADDR_IN);
	SOCKET ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientSockAddrIn, &ClientSockAddrInSize);
	
	//char Buffer[1024] = { 0, }; 이게 성능은 좋다.
	int X = 1;
	int Y = 1;
	bool bRunning = true;
	while (bRunning)
	{
		char Buffer[1024] = { 0, }; // 수업이니까 안에 넣어서 
		//blocking, non blocking    원래는 블록킹을 하지 않는다. cpu 손실이 나기 떄문에 따라서 주로 non blocking으로 만든다.
		//packit 설계부터 해야된다. protocol, 
		int RecvLength = recv(ClientSocket, Buffer, sizeof(Buffer), 0);

		if (RecvLength == 0)
		{
			//연결종료
			std::cout << "Disconnected Client" << std::endl;
			bRunning = false; 
			break;
		}
		else if (RecvLength < 0)
		{
			//error
			std::cout << "Disconnected Client By Error" << GetLastError() <<std::endl;
			bRunning = false;
			break;
		}

		//packet parse
		char KeyCode = Buffer[0];
		switch (KeyCode)
		{
		case 'w' : 
			if (Map[Y-1][X] == '1')
			{
				Y--;
			}
			break;
		case 's':
			if (Map[Y+1][X] == '1')
			{
				Y++;
			}
			break;
		case 'a':
			if (Map[Y][X-1] == '1')
			{
				X--;
			}
			break;
		case 'd':
			if (Map[Y][X+1] == '1')
			{
				X++;
			}
			break;
		}
		//std::cout << "(" << X << "," << Y << ")" << std::endl;
		system("cls");
		for (int  MapY = 0; MapY < 10; ++MapY)
		{
			for (int MapX= 0; MapX < 10; ++MapX)
			{
				if (Map[MapY][MapX] == '0')
				{
					std::cout << "#" << " "; 
				}
				else if (MapX == X && MapY == Y)
				{
					std::cout << "P" << " ";
				}
				else
				{
					std::cout << " " << " ";
				}
			}
			std::cout << std::endl;
		}
		MyMemCopy(&Buffer[0], (char*)&X, 4);
		MyMemCopy(&Buffer[4], (char*)&Y, 4);

		int SendLength = send(ClientSocket, Buffer, sizeof(Buffer), 0);
	}
	
	closesocket(ClientSocket);

	closesocket(ServerSocket);

	WSACleanup();
	return 0;
}