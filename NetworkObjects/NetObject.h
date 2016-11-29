#pragma once
#include <string>

class NetObject
{
public:
	virtual void call(std::string str) = 0;
	NetObject();
	virtual ~NetObject();
};

