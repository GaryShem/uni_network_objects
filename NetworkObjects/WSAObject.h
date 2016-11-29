#pragma once

#include <Winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>

class WSAObject
{
public:
	WSAObject();
	~WSAObject();
private:
	WSADATA wsaData;
};

