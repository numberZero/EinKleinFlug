#include "base.hxx"
#include <cerrno>
#include <string>
#include <system_error>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#ifdef WIN32
struct WSA
{
	static WSAData info;

	WSA()
	{
		int error = WSAStartup(0x0202, &info);
		if(error)
			throw std::system_error(error, std::system_category(), "Can’t initialize WSA: " + std::to_string(error));
	}

	~WSA()
	{
		WSACleanup();
	}
};

WSAData WSA::info;
static WSA _wsa_initializer;
#endif

void readData(int fd, Byte *block, size_t size)
{
	errno = 0;
	while(size > 0)
	{
		ssize_t result = recv(fd, reinterpret_cast<char *>(block), size, 0);
		if(result == 0)
			throw GracefulShutdown();
		if(result <= 0)
			net_error("Can’t read data");
		size -= result;
		block += result;
	}
}

void writeData(int fd, Byte const *block, size_t size)
{
	errno = 0;
	while(size > 0)
	{
		ssize_t result = send(fd, reinterpret_cast<char const *>(block), size, 0);
		if(result == 0)
			throw GracefulShutdown();
		if(result <= 0)
			net_error("Can’t write data");
		size -= result;
		block += result;
	}
}

void __attribute__((noreturn)) net_error(std::string const &text)
{
#ifdef WIN32
	throw std::system_error(WSAGetLastError(), std::system_category(), text);
#else
	throw std::system_error(errno, std::generic_category(), text);
#endif
}

#ifdef NDEBUG
void net_warning(std::string const &)
{
}
#else
void net_warning(std::string const &text)
{
	std::cerr << "Warning: network function failed, but no exception requested." << std::endl;
	std::cerr << "  " << text << ": " << std::generic_category().message(errno) << std::endl;
}
#endif
