#include "explosion.hxx"
#include <ctime>
#include <random>
#include "world.hxx"

Explosion::Explosion(World *world, PointState const &base, Float power) :
	ParticleSystem(world, 2.0),
	base_vel(2.5 * std::pow(power, 0.25)),
	base_life(0.5 * std::log(power + 1.0) + 0.1)
{
	auto seed = std::time(nullptr);
	std::ranlux24 gen(seed);
	static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
	static std::uniform_real_distribution<Float> rad(0.0, 1.0);
	std::uniform_real_distribution<Float> vel(0.0, 2.0 * base_vel);
	static std::uniform_real_distribution<Float> dlife(0.9, 1.1);
	while(power > 0)
	{
		Particle p;
		Float a = phi(gen);
		Float r = rad(gen);

		Float b = phi(gen);
		Float v = vel(gen);

		p.pos = base.pos + r * Vector2{ std::cos(a), std::sin(a) };
		p.vel = base.vel + v * Vector2{ std::cos(b), std::sin(b) };
		p.value = base_vel / (v + 10.0);
		p.life = base_life * (0.5 + 0.025 * v) * dlife(gen);
		particles.push_back(p);
		power -= p.value;
	}
}

void Explosion::create(World *world, PointState const &base, Float power)
{
	world->particles.emplace(new Explosion(world, base, power));
}

Color Explosion::getColor(Particle const &p)
{
	Float v = p.value;
	Float w = std::pow(p.life / base_life, 3.0);
	return Color(w, w * (3.0 * v - 1.0), w * (4.0 * v - 2.0));
}
