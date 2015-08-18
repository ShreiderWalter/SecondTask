#ifndef SERVER_H
#define SERVER_H

#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <string>
#include <vector>

#define BUFFER_SIZE 1024

class Server
{
private:
	SharedMemoryManager * manager;

public:
	Server();
	~Server();
	void run();
};

#endif //SERVER_H
