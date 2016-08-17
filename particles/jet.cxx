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
	emitter->spread_position = 0.2;
	emitter->spread_velocity = 0.2 * base_vel;
	emitter->spread_life = 0.1;
	ship.world->entities.insert(emitter);
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

/*
void Jet::generate(std::shared_ptr<Ship> ship)
{
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
	std::uniform_real_distribution<Float> dv(0.0, vel_spread);
	std::uniform_real_distribution<Float> dp(0.0, pos_spread);
	std::uniform_real_distribution<Float> ddt(-0.5 * world->dt, 0.5 * world->dt);
	static std::uniform_real_distribution<Float> dlife(0.9, 1.1);
	power = std::min(1.0, std::max(0.0, power));
	energy += power * world->dt;
	Float dvel = std::sqrt(power);
	while(energy > 0.0)
	{
		energy -= particle_energy;
		Particle p;
		p.pos = pos;
		p.vel = dvel * part_vel;
		Float a = phi(gen);
		Float r = dv(gen);
		p.vel[0] += r * std::cos(a);
		p.vel[1] += r * std::sin(a);
		a = phi(gen);
		r = dp(gen);
		p.pos[0] += r * std::cos(a);
		p.pos[1] += r * std::sin(a);
		p.pos += ddt(gen) * p.vel;
		p.value = particle_energy * p.vel.dot(part_vel) / (dvel * base_vel * base_vel);
		p.life = dlife(gen) * base_life * p.value / particle_energy;
		world->manifold.absolutize(*ship, p);
		particles.push_back(p);
	}
}

Color Jet::getColor(Particle const &p)
{
	Float w = std::pow(p.life / base_life, 3.0);
	Float v = p.value * w / particle_energy;
	return Color(2.5 * v, 2.5 * v - 1.0, 5.0 * v - 4.0);
}
*/