#include "server.hxx"
#include <cmath>
#include <cstring>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <unistd.h>
#include "base.hxx"
#include "protocol.hxx"
#include "ship.hxx"
#include "world.hxx"

Server::Server(World *world):
	world(world),
	connection_socket(-1)
{
	int opt = 1;
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(37771);
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(server_socket == -1)
		net_error("Can’t create socket");
	net_call_nt(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)));
	net_call(bind(server_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)));
	net_call(listen(server_socket, 1));
}

Server::~Server()
{
	net_call_nt(close(connection_socket));
	net_call_nt(close(server_socket));
}

void Server::accept1()
{
	connection_socket = accept(server_socket, nullptr, nullptr);
	if(server_socket == -1)
		net_error("Can’t accept connection");
	net_call_nt(close(server_socket));
}

void Server::sendState()
{
	FrameHeader hdr;
	hdr.key = 0x88C4C488;
	hdr.frame_id = world->frame;
	hdr.ship_count = world->ships.size();
	writeObject(connection_socket, hdr);
	for(std::shared_ptr<Ship const> ship: world->ships)
	{
		ShipHeader hdr;
		ShipDesc desc;
		ShipState state;
		hdr.desc_present = 1;
		hdr.state_present = 1;
		desc.recharge_rate = ship->recharge_rate;
		desc.max_hp = ship->max_hp;
		desc.armor = ship->armor;
		state.position[0] = ship->pos[0];
		state.position[1] = ship->pos[1];
		state.position[2] = ship->rpos;
		state.velocity[0] = ship->vel[0];
		state.velocity[1] = ship->vel[1];
		state.velocity[2] = ship->rvel;
		state.hp = ship->hp;
		writeObject(connection_socket, hdr);
		writeObject(connection_socket, desc);
		writeObject(connection_socket, state);
	}
	FrameFooter ftr;
	ftr.key = 0xAAC4C4AA;
	ftr.frame_id = world->frame;
	writeObject(connection_socket, ftr);
}
