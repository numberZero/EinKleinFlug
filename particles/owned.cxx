#include "owned.hxx"
#include "ship.hxx"

OwnedParticleSystem::OwnedParticleSystem(std::shared_ptr<Ship> ship, Float particle_size) :
	ParticleSystem(ship->world, particle_size),
	ship(ship)
{
}

void OwnedParticleSystem::detach()
{
	ship.reset();
}

void OwnedParticleSystem::move()
{
	ParticleSystem::move();
	std::shared_ptr<Ship> pship(ship.lock());
	if(pship)
		generate(pship);
}

bool OwnedParticleSystem::viable() const
{
	return !ship.expired() || ParticleSystem::viable();
}
