#include "client.hxx"
#include <cmath>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <unistd.h>
#include "base.hxx"
#include "protocol.hxx"
#include "ship.hxx"
#include "world.hxx"

extern std::shared_ptr<Ship> me;

Client::Client(World *world):
	world(world)
{
	connection_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(connection_socket == -1)
		net_error("Can’t create socket");
}

Client::~Client()
{
	net_call_nt(close(connection_socket));
}

void Client::connectLB()
{
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(37771);
	net_call(connect(connection_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)));
}

void Client::recvState()
{
	FrameHeader hdr;
	readObject(connection_socket, hdr);
	if(hdr.key != 0x88C4C488)
		throw std::runtime_error("Protocol error: invalid header key");
	if(hdr.frame_id != world->frame)
	{
		std::clog << "Warn: synchronization error detected" << std::endl;
	}
	world->ships.clear();
	for(unsigned k = 0; k != hdr.ship_count; ++k)
	{
		ShipHeader head;
		ShipDesc desc;
		ShipState state;
		readObject(connection_socket, head);
		readObject(connection_socket, desc);
		readObject(connection_socket, state);
		std::shared_ptr<Ship> ship(Ship::create(world, head.id));
		ship->recharge_rate = desc.recharge_rate;
		ship->max_hp = desc.max_hp;
		ship->armor = desc.armor;
		ship->pos[0] = state.position[0];
		ship->pos[1] = state.position[1];
		ship->rpos = state.position[2];
		ship->vel[0] = state.velocity[0];
		ship->vel[1] = state.velocity[1];
		ship->rvel = state.velocity[2];
		ship->mirror = state.mirror;
		ship->hp = state.hp;
		if(ship->id == hdr.your_id)
			me = ship;
	}
	FrameFooter ftr;
	readObject(connection_socket, ftr);
	if(ftr.key != 0xAAC4C4AA)
		throw std::runtime_error("Protocol error: invalid footer key");
	if(ftr.frame_id != hdr.frame_id)
		throw std::runtime_error("Server/protocol error: frame ID mismatch");
	world->frame = hdr.frame_id + 1;
}
