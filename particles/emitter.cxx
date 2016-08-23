#include "emitter.hxx"
#include <cmath>
#include <ctime>
#include <random>
#include "system.hxx"
#include "world.hxx"

extern World world;

PRNG seeder(std::time(nullptr));

ParticleEmitter::ParticleEmitter(BodyState &base) :
	generator(seeder()),
	base(base)
{
}

void ParticleEmitter::step()
{
	if(power <= 0 && energy <= 0)
		return;

	std::uniform_real_distribution<Float> distr_dt(0.0 * world.dt, 1.0 * world.dt);
	std::uniform_real_distribution<Float> distr_angle(-M_PI, M_PI);

	std::uniform_real_distribution<Float> distr_position(0.0, spread_position);
	std::uniform_real_distribution<Float> distr_velocity(0.0, spread_velocity);
	std::uniform_real_distribution<Float> distr_life(std::exp(-spread_life), std::exp(spread_life));

	std::shared_ptr<ParticleSystem> psys(particle_system.lock());
	if(!psys)
	{
		psys.reset(new ParticleSystem(&world, particle_size, colorization));
		world.particles.insert(psys);
		particle_system = psys;
	}

	Float value_coef = base_value / base_velocity.squaredNorm();

	energy += power * world.dt;
	while(energy > 0.0)
	{
		Particle p;

		// sequential calls to the PRNG
		Float pos_a = distr_angle(generator);
		Float pos_r = distr_position(generator);
		Float vel_a = distr_angle(generator);
		Float vel_r = distr_velocity(generator);
		Float dt = distr_dt(generator);
		Float life = distr_life(generator);

		p.vel = base_velocity + directionVector(vel_a, vel_r);
		p.pos = base_position + directionVector(pos_a, pos_r) + dt * p.vel;
		p.value = value_coef * p.vel.dot(base_velocity);
		p.life = base_life * life;
		if(p.value < 0 || p.life < 0)
			continue;

		world.manifold.absolutize(base, p);
		psys->particles.push_back(p);
		energy -= p.value;
	}
}
