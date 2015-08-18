#ifndef SHARED_MEMORY_MANAGER
#define SHARED_MEMORY_MANAGER

#include <iostream>
#include <string>
#include "PipeStatic.h"
#include <Windows.h>
#include <tchar.h>

class SharedMemoryManager
{
private:
	SharedMemoryManager();
	static HANDLE sharedMemory;
	static HANDLE eventLock;
	static HANDLE eventWrite;
	static LPTSTR buffer;
	static HANDLE mutex;

	static void InitializeSecurityAttributesForEverybodyAccess(SECURITY_ATTRIBUTES * pSecurityAttributes, 
		SECURITY_DESCRIPTOR * pSecurityDescriptor);

public:
	static void create(std::string name);
	static HANDLE connect(std::string name);
	static void close();

	static int read();
	static void write(const char * tmp);
	static void wait();
};


#endif // SHARED_MEMORY_MANAGER