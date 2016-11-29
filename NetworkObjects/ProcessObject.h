#pragma once
#include "NetObject.h"
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>

class ProcessObject :
	public NetObject
{
public:
	ProcessObject();
	~ProcessObject();
	void call(std::string str) override;
};

