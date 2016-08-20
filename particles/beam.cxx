#include "beam.hxx"
#include "emitter.hxx"
#include <ctime>
#include <random>
#include "ship.hxx"
#include "world.hxx"

Beam::Beam(Ship &ship, Vector2 shift, Vector2 vel, Float power, Float range) :
	ship(ship),
	emitter(new ParticleEmitter(ship)),
	power(power)
{
	Float base_vel = vel.norm();
	emitter->particle_size = 0.4;
	emitter->base_position = shift;
	emitter->base_velocity = vel;
	emitter->base_life = range / base_vel;
	emitter->base_value = emitter->base_life / 400.0;
	emitter->spread_position = 0.1;
	emitter->spread_velocity = 0.04 * base_vel;
	emitter->spread_life = 0.1;
	ship.world->entities.insert(emitter);

	Float c = 1.0 / (emitter->base_value * std::pow(emitter->base_life, 3));
	emitter->colorization <<
		0.0, 0.0, 2.5 * c, -1.5,
		0.0, 0.0, 5.0 * c, -4.0,
		0.0, 0.0, 3.5 * c, 0.0,
		0.0, 0.0, 0.0, 1.0;
}

bool Beam::doesShot() const
{
	return shots;
}

void Beam::setShot(bool value)
{
	shots = value;
	emitter->power = shots ? power : 0;
}
