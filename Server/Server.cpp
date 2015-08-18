#include "Server.h"

Server::Server()
{
	SharedMemoryManager::create(MAPPING_OBJECT_NAME);
}

Server::~Server()
{
	SharedMemoryManager::close();
}

void threadProgress()
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
		std::string buf = std::to_string(color);
		prev_color = color;
		std::cout << buf << "\n";

		SharedMemoryManager::write(buf.c_str());
		SharedMemoryManager::wait();
		
	}
}

void Server::run()
{
	char * tmp = "Hello";
	SharedMemoryManager::write(tmp);
	SharedMemoryManager::wait();

	std::thread thread(threadProgress);
	thread.detach();

}

