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
	SharedMemoryManager(std::string name);
	HANDLE sharedMemory;
	HANDLE eventLock;
	HANDLE eventWrite;
	LPTSTR buffer;
	HANDLE mutex;
	static std::shared_ptr<SharedMemoryManager> self;

	static void InitializeSecurityAttributesForEverybodyAccess(SECURITY_ATTRIBUTES * pSecurityAttributes, 
		SECURITY_DESCRIPTOR * pSecurityDescriptor);

public:
	static std::shared_ptr<SharedMemoryManager> create(const std::string &name);
	static std::shared_ptr<SharedMemoryManager> connect(const std::string &name);
	void close();

	int read();
	void write(const char * tmp);
	void wait();
	bool isEmpty() const;
};


#endif // SHARED_MEMORY_MANAGER