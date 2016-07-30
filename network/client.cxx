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
}

Client::~Client()
{
	close(connection_socket);
}

void Client::connectLB()
{
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(37771);
	connect(connection_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
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
		ShipHeader hdr;
		ShipDesc desc;
		ShipState state;
		readObject(connection_socket, hdr);
		if(hdr.desc_present)
			readObject(connection_socket, desc);
		else
		{
			desc.recharge_rate = 0.5;
			desc.max_hp = 10.0;
			desc.armor = 7.0;
		}
		if(hdr.state_present)
			readObject(connection_socket, state);
		else
		{
			std::clog << "Stateless ship?!" << std::endl;
			state.position[0] = me->pos[0];
			state.position[1] = me->pos[1];
			state.position[2] = me->rpos;
			state.velocity[0] = me->vel[0];
			state.velocity[1] = me->vel[1];
			state.velocity[2] = me->rvel;
			state.hp = me->hp;
		}
		std::shared_ptr<Ship> ship(Ship::create(world, desc.max_hp, desc.armor));
		ship->recharge_rate = desc.recharge_rate;
		ship->pos[0] = state.position[0];
		ship->pos[1] = state.position[1];
		ship->rpos = state.position[2];
		ship->vel[0] = state.velocity[0];
		ship->vel[1] = state.velocity[1];
		ship->rvel = state.velocity[2];
		ship->hp = state.hp;
	}
	world->ships.emplace(me);
	FrameFooter ftr;
	readObject(connection_socket, ftr);
	if(ftr.key != 0xAAC4C4AA)
		throw std::runtime_error("Protocol error: invalid footer key");
	if(ftr.frame_id != hdr.frame_id)
		throw std::runtime_error("Server/protocol error: frame ID mismatch");
	world->frame = hdr.frame_id + 1;
}
