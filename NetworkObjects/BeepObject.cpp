#include "BeepObject.h"



BeepObject::BeepObject()
{
}


BeepObject::~BeepObject()
{
}

void BeepObject::call(std::string str)
{
	Beep(440, 2000);
}
