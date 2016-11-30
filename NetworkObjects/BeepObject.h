#pragma once
#include "NetObject.h"
#include "Windows.h"
class BeepObject :
	public NetObject
{
public:
	BeepObject();
	~BeepObject();
	void call(std::string str) override;
};

