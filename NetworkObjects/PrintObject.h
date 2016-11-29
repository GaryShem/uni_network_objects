#pragma once
#include "NetObject.h"
#include <iostream>
class PrintObject :
	public NetObject
{
public:
	PrintObject();
	~PrintObject();
	void call(std::string str) override;
};

