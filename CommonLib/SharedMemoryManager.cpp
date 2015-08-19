#include "SharedMemoryManager.h"

std::shared_ptr<SharedMemoryManager> SharedMemoryManager::self;

SharedMemoryManager::SharedMemoryManager()
{
}

SharedMemoryManager::SharedMemoryManager(std::string name)
{
	SECURITY_DESCRIPTOR sd;
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

std::shared_ptr<SharedMemoryManager> SharedMemoryManager::create(const std::string &name) 
{
	self = std::shared_ptr<SharedMemoryManager>(new SharedMemoryManager(name));
	return self;
}

std::shared_ptr<SharedMemoryManager> SharedMemoryManager::connect(const std::string &name)
{
	self = std::shared_ptr<SharedMemoryManager>(new SharedMemoryManager);
	self->sharedMemory = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, name.c_str());
	self->eventLock = OpenEvent(EVENT_ALL_ACCESS, false, EVENT_NAME);
	self->eventWrite = OpenEvent(EVENT_ALL_ACCESS, false, EVENT_WRITE_NAME);
	self->buffer = (LPSTR) MapViewOfFile(self->sharedMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, BUFFER_SIZE);
	std::cout << GetLastError();
	self->mutex = OpenMutex(FILE_MAP_READ | FILE_MAP_WRITE, false, MUTEX_NAME);

	SetEvent(self->eventLock);

	return self;
}

bool SharedMemoryManager::isEmpty() const
{
	return nullptr == sharedMemory;
}

void SharedMemoryManager::close()
{
	CloseHandle(sharedMemory);
}

void SharedMemoryManager::write(const char * tmp)
{
	WaitForSingleObject(mutex, INFINITE);
	CopyMemory((PVOID)buffer, tmp, (_tcslen(tmp)) * sizeof(TCHAR));
	ReleaseMutex(mutex);
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
