#include "PrintObject.h"

PrintObject::PrintObject(){}

PrintObject::~PrintObject(){}

void PrintObject::call(std::string str)
{
	if (str.empty())
	{
		std::cout << "This is a default message for PrintObject" << std::endl;
	}
	else
	{
		std::cout << str << std::endl;
	}
}
