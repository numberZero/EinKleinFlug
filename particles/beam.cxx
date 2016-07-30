#include "beam.hxx"
#include <ctime>
#include <random>
#include "ship.hxx"
#include "world.hxx"

std::shared_ptr<Beam> Beam::create(std::shared_ptr<Ship> ship, Vector2 shift, Vector2 vel, Float power, Float range)
{
	std::shared_ptr<Beam> beam(new Beam(ship, 0.4));
	beam->power = power;
	beam->vel = vel;
	beam->pos = shift;
	beam->base_vel = beam->vel.norm();
	beam->base_life = range / beam->base_vel;
	beam->pos_spread = 0.3;
	beam->vel_spread = 0.04 * beam->base_vel;
	beam->size_fullpower = 400.0 * beam->power;
	beam->particle_energy = beam->base_life * beam->power / beam->size_fullpower;
	beam->world->particles.emplace(beam);
	return beam;
}

// particels / second = power / particle_energy
// size_fullpower = base_life * particels / second = base_life * power / particle_energy
// particle_energy = base_life * power / size_fullpower

void Beam::generate(std::shared_ptr<Ship> ship)
{
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
	std::uniform_real_distribution<Float> dv(0.0, vel_spread);
	std::uniform_real_distribution<Float> dp(0.0, pos_spread);
	std::uniform_real_distribution<Float> ddt(-0.5 * world->dt, 0.5 * world->dt);
	static std::uniform_real_distribution<Float> dlife(0.9, 1.1);
	if(shots)
		energy += power * world->dt;
	while(energy > 0.0)
	{
		energy -= particle_energy;
		Particle p;
		p.pos = pos;
		p.vel = vel;
		Float a = phi(gen);
		Float r = dv(gen);
		p.vel[0] += r * std::cos(a);
		p.vel[1] += r * std::sin(a);
		a = phi(gen);
		r = dp(gen);
		p.pos[0] += r * std::cos(a);
		p.pos[1] += r * std::sin(a);
		p.pos += ddt(gen) * p.vel;
		p.value = particle_energy * p.vel.dot(vel) / (base_vel * base_vel);
		p.life = dlife(gen) * base_life * p.value / particle_energy;
		world->manifold.absolutize(*ship, p);
		particles.push_back(p);
	}
}

Color Beam::getColor(Particle const &p)
{
	Float w = std::pow(p.life / base_life, 3.0);
	Float v = p.value * w / particle_energy;
	return Color(2.5 * v - 1.0, 5.0 * v - 4.0, 2.5 * v);
}
