#include "WSAObject.h"

WSAObject::WSAObject()
{
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		exit(255);
	}
	std::cout << "WSA initialized" << std::endl;
}


WSAObject::~WSAObject()
{
	WSACleanup();
}
