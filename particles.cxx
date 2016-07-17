#include "particles.hxx"
#include <ctime>
#include <random>
#include <GL/gl.h>
#include "world.hxx"

ParticleSystem::ParticleSystem(World *world, double particle_size) :
	world(world),
	particle_size(particle_size)
{
	world->particles.insert(this);
}

bool ParticleSystem::viable() const
{
	return !particles.empty();
}

void ParticleSystem::die()
{
}

void ParticleSystem::move(double dt)
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
		double d = part.scale.squaredNorm();
		double m[16] = {
			part.scale[0], part.scale[1], 0.0, 0.0,
			-part.scale[1] / d, part.scale[0] / d, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0,
		};
		glMultMatrixd(m);
*/
		draw1(part);
		glPopMatrix();
	}
	glPopMatrix();
}

void ParticleSystem::draw1(Particle const &p)
{
	long const q = 5;//16;
	double const dphi = 2.0 * M_PI / q;
	glBegin(GL_TRIANGLE_FAN);
	colorize(p);
	glVertex2d(0.0, 0.0);
	glColor4f(0.0, 0.0, 0.0, 1.0);
	for(long k = 0; k != q; ++k)
	{
		double r = particle_size;
		double phi = dphi * k;
		glVertex2d(r * std::cos(phi), r * std::sin(phi));
		glVertex2d(r * std::cos(phi + dphi), r * std::sin(phi + dphi));
	}
	glEnd();
}

void ParticleSystem::colorize(Particle const &p)
{
	double y = std::atan(p.life) / M_PI;
	glColor4f(y, y, y, 1.0);
}

Explosion::Explosion(World *world, PointState const &base, double power) :
	ParticleSystem(world, 2.0),
	base_vel(2.5 * std::pow(power, 0.25)),
	base_life(0.5 * std::log(power + 1.0) + 0.1)
{
	auto seed = std::time(nullptr);
	std::ranlux24 gen(seed);
	static std::uniform_real_distribution<double> phi(-M_PI, M_PI);
	static std::uniform_real_distribution<double> rad(0.0, 1.0);
	std::uniform_real_distribution<double> vel(0.0, 2.0 * base_vel);
	static std::uniform_real_distribution<double> dlife(0.9, 1.1);
	while(power > 0)
	{
		Particle p;
		double a = phi(gen);
		double r = rad(gen);

		double b = phi(gen);
		double v = vel(gen);

		p.pos = base.pos + r * Eigen::Vector2d{ std::cos(a), std::sin(a) };
		p.vel = base.vel + v * Eigen::Vector2d{ std::cos(b), std::sin(b) };
		p.value = base_vel / (v + 10.0);
		p.life = base_life * (0.5 + 0.025 * v) * dlife(gen);
		particles.push_back(p);
		power -= p.value;
	}
}

void Explosion::colorize(Particle const &p)
{
	double v = p.value;
	double w = std::pow(p.life / base_life, 3.0);
	glColor4f(w, w * (3.0 * v - 1.0), w * (4.0 * v - 2.0), 1.0);
}

Jet::Jet(World *world, BodyState const &parent, PointState const &shift, double full_thrust, double visual_scale, double 
visual_density):
	ParticleSystem(world, 0.5),
	full_thrust(full_thrust),
	base_vel(visual_scale * shift.vel.norm()),
	base_life(0.02 / visual_scale),
	pos_spread(0.2),
	vel_spread(0.2 * base_vel),
	size_fullpower(150.0 * visual_density),
	particle_energy(base_life / size_fullpower),
	parent(parent),
	shift{shift.pos, visual_scale * shift.vel}
{
}

bool Jet::viable() const
{
	return true; // permanent PS
}

void Jet::move(double dt)
{
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<double> phi(-M_PI, M_PI);
	std::uniform_real_distribution<double> dv(0.0, vel_spread);
	std::uniform_real_distribution<double> dp(0.0, pos_spread);
	std::uniform_real_distribution<double> ddt(-0.5 * dt, 0.5 * dt);
	static std::uniform_real_distribution<double> dlife(0.9, 1.1);
	energy += power * dt;
	while(energy > 0.0)
	{
		energy -= particle_energy;
		Particle p;
		(PointState &)p = shift;
		double a = phi(gen);
		double r = dv(gen);
		p.vel[0] += r * std::cos(a);
		p.vel[1] += r * std::sin(a);
		a = phi(gen);
		r = dp(gen);
		p.pos[0] += r * std::cos(a);
		p.pos[1] += r * std::sin(a);
		p.pos += ddt(gen) * p.vel;
		p.value = p.vel.dot(shift.vel) / (base_vel * base_vel);
		p.life = dlife(gen) * base_life * p.value;
		world->manifold.absolutize(parent, p);
		particles.push_back(p);
	}
	ParticleSystem::move(dt);
}

void Jet::colorize(Particle const &p)
{
	double w = std::pow(p.life / base_life, 3.0);
	double v = p.value * w;
	glColor4f(2.5 * v, 2.5 * v - 1.0, 5.0 * v - 4.0, 1.0);
}
