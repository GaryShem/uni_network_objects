#include "ProcessObject.h"



ProcessObject::ProcessObject()
{
}


ProcessObject::~ProcessObject()
{
}

void ProcessObject::call(std::string str)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "error listing processes" << std::endl;
		return;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		std::cout << "error listing processes" << std::endl;
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return;
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	std::cout << std::endl;
	do
	{
		_tprintf(TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile);
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
}
