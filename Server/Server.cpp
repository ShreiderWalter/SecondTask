#include "Server.h"

Server::Server()
{
	manager = SharedMemoryManager::create(MAPPING_OBJECT_NAME).get();
}

Server::~Server()
{
	manager->close();
}

void threadProgress(SharedMemoryManager * manager)
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

		manager->write(buf.c_str());
		manager->wait();
		
	}
}

void Server::run()
{
	char * tmp = "Hello";
	manager->write(tmp);
	manager->wait();

	std::thread thread(threadProgress, manager);
	thread.join();

}

