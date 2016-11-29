#pragma once
#include "NetObject.h"
class NoneObject :
	public NetObject
{
public:
	NoneObject();
	~NoneObject();
	void call(std::string str) override;
};

