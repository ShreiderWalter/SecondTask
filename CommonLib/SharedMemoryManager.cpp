#include "SharedMemoryManager.h"

HANDLE SharedMemoryManager::sharedMemory = nullptr;
HANDLE SharedMemoryManager::eventLock = nullptr;
HANDLE SharedMemoryManager::eventWrite = nullptr;
LPTSTR SharedMemoryManager::buffer = nullptr;
HANDLE SharedMemoryManager::mutex = nullptr;

void SharedMemoryManager::InitializeSecurityAttributesForEverybodyAccess(SECURITY_ATTRIBUTES * pSecurityAttributes, 
		SECURITY_DESCRIPTOR * pSecurityDescriptor)
{
	memset(pSecurityAttributes, NULL, sizeof(SECURITY_ATTRIBUTES));
	pSecurityAttributes->bInheritHandle = true;
	pSecurityAttributes->lpSecurityDescriptor = pSecurityDescriptor;
	pSecurityAttributes->nLength = sizeof(SECURITY_ATTRIBUTES);
	InitializeSecurityDescriptor(pSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(pSecurityDescriptor, true, nullptr, false);
}

void SharedMemoryManager::create(std::string name) 
{
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

	SECURITY_ATTRIBUTES sa;
	InitializeSecurityAttributesForEverybodyAccess(&sa, &sd);


	sharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE,
						&sa,
						PAGE_READWRITE,
						0,
						BUFFER_SIZE,
						name.c_str());

	eventLock = CreateEvent(nullptr, false, false, EVENT_NAME);

	eventWrite = CreateEvent(nullptr, false, false, EVENT_WRITE_NAME);

	buffer = (LPTSTR) MapViewOfFile(sharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);

	mutex = CreateMutex(nullptr, FALSE, MUTEX_NAME);

}

void SharedMemoryManager::close()
{
	CloseHandle(sharedMemory);
}

HANDLE SharedMemoryManager::connect(std::string name)
{
	sharedMemory = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, name.c_str());
	eventLock = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_NAME);
	eventWrite = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENT_WRITE_NAME);
	buffer = (LPTSTR) MapViewOfFile(sharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
	mutex = OpenMutex(FILE_MAP_ALL_ACCESS, FALSE, MUTEX_NAME);

	SetEvent(eventLock);

	return sharedMemory;
}

void SharedMemoryManager::write(const char * tmp)
{
	//CopyMemory((PVOID)buffer, msg.c_str(), (_tcslen(msg.c_str()) * sizeof(TCHAR)));
	//SetEvent(eventLock);

	CopyMemory((PVOID)buffer, tmp, (_tcslen(tmp)) * sizeof(TCHAR));
	SetEvent(eventLock);
}

int SharedMemoryManager::read()
{
	WaitForSingleObject(eventLock, INFINITE);
	WaitForSingleObject(mutex, INFINITE);
	char * tmp = (char *) buffer;
	std::cout << tmp << std::endl;
	ReleaseMutex(mutex);
	SetEvent(eventWrite);

	return (int)tmp[0];
}

void SharedMemoryManager::wait()
{
	WaitForSingleObject(eventWrite, INFINITE);
}
