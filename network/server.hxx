#pragma once
#include "types.hxx"

class World;

struct Server
{
	World *const world;
	int server_socket;
	int connection_socket;

	Server(World *world);
	~Server();
	void accept1();
	void sendState(Id his_id);
};
