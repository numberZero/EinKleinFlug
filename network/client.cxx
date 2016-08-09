#include "client.hxx"
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#endif
#include <unistd.h>
#include "base.hxx"
#include "particles/base.hxx"
#include "particles/beam.hxx"
#include "particles/jet.hxx"
#include "particles/explosion.hxx"
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
	net_call_nt(closesocket(connection_socket));
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
	std::map<Id, std::shared_ptr<Ship>> ships;
	for(auto pship: world->ships)
		ships.emplace(pship->id, std::move(pship));
	world->ships.clear();
	for(unsigned k = 0; k != hdr.ship_count; ++k)
	{
		ShipHeader head;
		ShipDesc desc;
		ShipState state;
		readObject(connection_socket, head);
		readObject(connection_socket, desc);
		readObject(connection_socket, state);
		std::shared_ptr<Ship> &ship(ships[head.id]);
		if(ship)
			world->ships.insert(ship);
		else
			ship = Ship::create(world, head.id);
		ship->radius = desc.radius;
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
	}
	me = ships[hdr.your_id];
	std::shared_ptr<Beam> beam = me->beams[0];
	std::shared_ptr<Jet> jet = me->jets[0];
	static std::shared_ptr<Explosion> explosion;
	if(!explosion)
	{
		world->particles.clear();
		Explosion::create(world, *me.get(), 200.0);
		explosion = std::dynamic_pointer_cast<Explosion>(*world->particles.begin()); // dirty hack!
		if(!explosion)
			throw std::logic_error("Explosion hack didn’t worked");
		world->particles.insert(beam);
		world->particles.insert(jet);
	}
	else
		world->particles.insert(explosion); // another dirty hack
	beam->particles.clear();
	jet->particles.clear();
	explosion->particles.clear();
	for(unsigned k = 0; k != hdr.psys_count; ++k)
	{
		PSysHeader head;
		readObject(connection_socket, head);
		std::shared_ptr<ParticleSystem> psys;
		switch(head.type)
		{
			case PSysType::Beam:
				psys = beam;
				break;
			case PSysType::Explosion:
				psys = explosion;
				break;
			case PSysType::Jet:
				psys = jet;
				break;
			default:
				throw std::runtime_error("Protocol error: unknown particle system type");
		}
		for(unsigned j = 0; j != head.particle_count; ++j)
		{
			ParticleState state;
			readObject(connection_socket, state);
			Particle part;
			part.pos[0] = state.position[0];
			part.pos[1] = state.position[1];
			part.vel[0] = state.velocity[0];
			part.vel[1] = state.velocity[1];
			part.life = state.life;
			part.value = state.value;
			psys->particles.push_back(part);
		}
	}
	FrameFooter ftr;
	readObject(connection_socket, ftr);
	if(ftr.key != 0xAAC4C4AA)
		throw std::runtime_error("Protocol error: invalid footer key");
	if(ftr.frame_id != hdr.frame_id)
		throw std::runtime_error("Server/protocol error: frame ID mismatch");
	world->frame = hdr.frame_id + 1;
}
