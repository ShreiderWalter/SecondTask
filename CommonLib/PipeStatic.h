#include <string>
#include <algorithm>
#include <memory>

namespace PipeLine
{
	#define PIPE_NAME TEXT("\\\\.\\pipe\\Pipe")
	#define BUFFER_SIZE 1024
	#define MAPPING_OBJECT_NAME TEXT("MyMappingObject")
	#define EVENT_NAME TEXT("Global\\AcceptLock")
	#define EVENT_WRITE_NAME TEXT("Global\\WriteLock")
	#define MUTEX_NAME TEXT("Global\\MutexLock")

	class Message
	{
	private:
		char * buffer;
		int curr_length;

	public:
		Message();
		Message(char * msg, int length);
		Message(const Message & other);
		Message(Message && other);
		~Message();

		Message & operator = (const Message & other);
		Message & operator = (Message && other);

		char * getBuffer() const;
	};
}