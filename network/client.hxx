#pragma once

class World;

struct Client
{
	World *const world;
	int connection_socket;

	Client(World *world);
	~Client();
	void connectLB();
	void recvState();
};
