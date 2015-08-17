#include <string>
#include <algorithm>

namespace PipeLine
{
	#define PIPE_NAME TEXT("\\\\.\\pipe\\Pipe")
	#define BUFFER_SIZE 1024
	#define MAPPING_OBJECT_NAME TEXT("Global\\MyMappingObject")

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