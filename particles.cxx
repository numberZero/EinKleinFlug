#include "particles.hxx"
#include <ctime>
#include <random>
#include <GL/gl.h>
#include "ship.hxx"
#include "world.hxx"

ParticleSystem::ParticleSystem(World *world, Float particle_size) :
	world(world),
	particle_size(particle_size)
{
	world->particles.insert(this);
	drawlist = glGenLists(1);
	glNewList(drawlist, GL_COMPILE);
	draw1();
	glEndList();
}

ParticleSystem::~ParticleSystem()
{
	glDeleteLists(drawlist, 1);
}

bool ParticleSystem::viable() const
{
	return !particles.empty();
}

void ParticleSystem::move(Float dt)
{
	for(auto iter = particles.begin(); iter != particles.end(); )
	{
		auto ppart = iter++;
		world->manifold.stepState(*ppart, dt);
		ppart->life -= dt;
		if(ppart->life <= 0)
			particles.erase(ppart);
	}
}

void ParticleSystem::draw(BodyState const *base)
{
	glPushMatrix();
	glRotated(-180.0 / M_PI * base->rpos, 0.0, 0.0, 1.0);
	if(base->mirror)
		glScaled(-1.0, 1.0, 1.0);
	glTranslated(-base->pos[0], -base->pos[1], 0.0);
	for(Particle const &part: particles)
	{
		PointState pt = part;
		world->manifold.remap(*base, pt);
		glPushMatrix();
		glTranslated(pt.pos[0], pt.pos[1], 0.0);
/*
		Float d = part.scale.squaredNorm();
		Float m[16] = {
			part.scale[0], part.scale[1], 0.0, 0.0,
			-part.scale[1] / d, part.scale[0] / d, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0,
		};
		glMultMatrixd(m);
*/
		colorize(part);
		glCallList(drawlist);
		glPopMatrix();
	}
	glPopMatrix();
}

void ParticleSystem::draw1()
{
	long const q = 5;//16;
	Float const dphi = 2.0 * M_PI / q;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(0.0, 0.0);
	glColor4f(0.0, 0.0, 0.0, 1.0);
	for(long k = 0; k != q; ++k)
	{
		Float r = particle_size;
		Float phi = dphi * k;
		glVertex2d(r * std::cos(phi), r * std::sin(phi));
		glVertex2d(r * std::cos(phi + dphi), r * std::sin(phi + dphi));
	}
	glEnd();
}

void ParticleSystem::colorize(Particle const &p)
{
	Float y = std::atan(p.life) / M_PI;
	glColor4f(y, y, y, 1.0);
}

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

void Explosion::colorize(Particle const &p)
{
	Float v = p.value;
	Float w = std::pow(p.life / base_life, 3.0);
	glColor4f(w, w * (3.0 * v - 1.0), w * (4.0 * v - 2.0), 1.0);
}

Jet::Jet(Ship *ship, Vector2 shift, Vector2 thrust):
	ParticleSystem(ship->world, 0.5),
	full_thrust(thrust.norm()),
	base_vel(25.0),
	base_life(0.4),
	pos_spread(0.2),
	vel_spread(0.2 * base_vel),
	size_fullpower(150.0),
	particle_energy(base_life / size_fullpower),
	part_vel(-base_vel / full_thrust * thrust),
	ship(ship),
	pos(shift),
	thrust(thrust)
{
}

bool Jet::viable() const
{
	return !!ship; // permanent PS
}

void Jet::die()
{
	ship = nullptr;
}

void Jet::move(Float dt)
{
	ParticleSystem::move(dt);
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
	std::uniform_real_distribution<Float> dv(0.0, vel_spread);
	std::uniform_real_distribution<Float> dp(0.0, pos_spread);
	std::uniform_real_distribution<Float> ddt(-0.5 * dt, 0.5 * dt);
	static std::uniform_real_distribution<Float> dlife(0.9, 1.1);
	power = std::min(1.0, std::max(0.0, power));
	energy += power * dt;
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

void Jet::colorize(Particle const &p)
{
	Float w = std::pow(p.life / base_life, 3.0);
	Float v = p.value * w / particle_energy;
	glColor4f(2.5 * v, 2.5 * v - 1.0, 5.0 * v - 4.0, 1.0);
}

Beam::Beam(Ship *ship, Vector2 shift, Vector2 vel, Float power, Float range):
	ParticleSystem(ship->world, 0.5),
	base_vel(vel.norm()),
	base_life(range / base_vel),
	pos_spread(0.7),
	vel_spread(0.07 * base_vel),
	size_fullpower(800.0 * power),
	particle_energy(base_life * power / size_fullpower),
	ship(ship),
	pos(shift),
	vel(vel),
	power(power)
{
}

// particels / second = power / particle_energy
// size_fullpower = base_life * particels / second = base_life * power / particle_energy
// particle_energy = base_life * power / size_fullpower

bool Beam::viable() const
{
	return !!ship; // permanent PS
}

void Beam::die()
{
	ship = nullptr;
}

void Beam::move(Float dt)
{
	ParticleSystem::move(dt);
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
	std::uniform_real_distribution<Float> dv(0.0, vel_spread);
	std::uniform_real_distribution<Float> dp(0.0, pos_spread);
	std::uniform_real_distribution<Float> ddt(-0.5 * dt, 0.5 * dt);
	static std::uniform_real_distribution<Float> dlife(0.9, 1.1);
	if(shots)
		energy += power * dt;
	Float dvel = std::sqrt(power);
	while(energy > 0.0)
	{
		energy -= particle_energy;
		Particle p;
		p.pos = pos;
		p.vel = dvel * vel;
		Float a = phi(gen);
		Float r = dv(gen);
		p.vel[0] += r * std::cos(a);
		p.vel[1] += r * std::sin(a);
		a = phi(gen);
		r = dp(gen);
		p.pos[0] += r * std::cos(a);
		p.pos[1] += r * std::sin(a);
		p.pos += ddt(gen) * p.vel;
		p.value = particle_energy * p.vel.dot(vel) / (dvel * base_vel * base_vel);
		p.life = dlife(gen) * base_life * p.value / particle_energy;
		world->manifold.absolutize(*ship, p);
		particles.push_back(p);
	}
}

void Beam::colorize(Particle const &p)
{
	Float w = std::pow(p.life / base_life, 3.0);
	Float v = p.value * w / particle_energy;
	glColor4f(2.5 * v - 1.0, 5.0 * v - 4.0, 2.5 * v, 1.0);
}
