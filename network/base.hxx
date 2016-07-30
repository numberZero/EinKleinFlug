#pragma once
#include <cstdint>
#include <type_traits>

typedef std::uint8_t Byte;

struct GracefulShutdown
{
};

void readData(int fd, Byte *block, std::size_t size);
void writeData(int fd, Byte const *block, std::size_t size);

template <typename T>
void readObject(int fd, T &object)
{
	static_assert(std::is_standard_layout<T>::value, "Only standard layout objects may be read from a socket/file");
	readData(fd, reinterpret_cast<Byte *>(&object), sizeof(T));
}

template <typename T>
void writeObject(int fd, T const &object)
{
	static_assert(std::is_standard_layout<T>::value, "Only standard layout objects may be written to a socket/file");
	writeData(fd, reinterpret_cast<Byte const *>(&object), sizeof(T));
}
