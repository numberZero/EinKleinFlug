#include "jet.hxx"
#include "emitter.hxx"
#include <ctime>
#include <random>
#include "ship.hxx"
#include "world.hxx"

Jet::Jet(Ship &ship, Vector2 shift, Vector2 thrust) :
	ship(ship),
	emitter(new ParticleEmitter(ship)),
	power(0.0),
	pos(shift),
	thrust(thrust)
{
	Float base_vel = 25.0;
	Float thrust_norm = thrust.norm();
	emitter->particle_size = 0.5;
	emitter->base_position = shift;
	emitter->base_velocity = -base_vel / thrust_norm * thrust;
	emitter->base_life = 0.4;
	emitter->base_value = emitter->base_life / 250.0;
	emitter->spread_position = 0.1;
	emitter->spread_velocity = 0.2 * base_vel;
	emitter->spread_life = 0.1;
	ship.world->entities.insert(emitter);

	Float c = 1.0 / (emitter->base_value * std::pow(emitter->base_life, 3));
	emitter->colorization <<
		0.0, 0.0, 1.0 * c, 0.0,
		0.0, 0.0, 2.5 * c, -0.6,
		0.0, 0.0, 5.0 * c, -2.4,
		0.0, 0.0, 0.0, 1.0;
}

Float Jet::getPower() const
{
	return power;
}

void Jet::setPower(Float value)
{
	power = std::min(1.0, std::max(0.0, value));
	emitter->power = power;
}
