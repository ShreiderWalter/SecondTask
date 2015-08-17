#include "Server.h"

Server::Server() : hPipe(nullptr), flag(true)
{
}

Server::~Server()
{
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
}

void threadProgress(HANDLE hPipe)
{ 
	std::cout << "Thread started ... \n";
	int prev_color = 0;
	while(true)
	{
		srand(time(nullptr));
		int color = rand() % 4 + 1;
		while(prev_color == color)
		{
			color = rand() % 4 + 1;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::string tmp = /*"lights up" + */std::to_string(color);
		prev_color = color;
		//std::cout << tmp << "\n";

		HANDLE eventLock = CreateEvent(nullptr, false, false, "Global\\AcceptLock");
		LPTSTR buffer = (LPTSTR) MapViewOfFile(hPipe, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
		WaitForSingleObject(eventLock, INFINITE);
		CopyMemory((PVOID)buffer, tmp.c_str(), (_tcslen(tmp.c_str()) * sizeof(TCHAR)));
		CloseHandle(eventLock);
		UnmapViewOfFile(buffer);

	}

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
}

void Server::run()
{
	hPipe = CreateFileMapping(INVALID_HANDLE_VALUE,
							nullptr,
							PAGE_READWRITE,
							0,
							BUFFER_SIZE,
							MAPPING_OBJECT_NAME);

	while(true)
	{
		if (hPipe == nullptr) 
		{
			return;
		}

		LPTSTR buffer = (LPTSTR) MapViewOfFile(hPipe, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
		if(buffer == nullptr)
		{
			CloseHandle(hPipe);
			return;
		}
		
		HANDLE eventLock = CreateEvent(nullptr, false, false, "Global\\SendLock");
		WaitForSingleObject(eventLock, INFINITE);
		CloseHandle(eventLock);
		UnmapViewOfFile(buffer);

		std::thread thread(threadProgress, hPipe);
		thread.detach();
	}
	CloseHandle(hPipe);

}

