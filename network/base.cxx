#include "base.hxx"
#include <cerrno>
#include <system_error>
#include <sys/socket.h>
#include <unistd.h>

void readData(int fd, Byte *block, size_t size)
{
	errno = 0;
	while(size > 0)
	{
		ssize_t result = read(fd, block, size);
		if(result == 0)
			throw GracefulShutdown();
		if(result <= 0)
			throw std::system_error(errno, std::generic_category(), "Can’t read data");
		size -= result;
		block += result;
	}
}

void writeData(int fd, Byte const *block, size_t size)
{
	errno = 0;
	while(size > 0)
	{
		ssize_t result = write(fd, block, size);
		if(result == 0)
			throw GracefulShutdown();
		if(result <= 0)
			throw std::system_error(errno, std::generic_category(), "Can’t write data");
		size -= result;
		block += result;
	}
}
