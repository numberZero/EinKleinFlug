#include "particles.hxx"
#include <ctime>
#include <random>
#include <GL/gl.h>
#include "world.hxx"

ParticleSystem::ParticleSystem(World *world) :
	world(world)
{
}

void ParticleSystem::move(double dt)
{
	for(auto iter = particles.begin(); iter != particles.end(); )
	{
		auto ppart = iter++;
		world->manifold.stepState(*ppart, dt);
		ppart->value -= dt;
		if(ppart->value <= 0)
			particles.erase(ppart);
	}
}

void ParticleSystem::draw(BodyState const *base)
{
	for(Particle const &part: particles)
	{
		PointState pt = part;
		world->manifold.relativize(*base, pt);
		glPushMatrix();
		glTranslated(pt.pos[0], pt.pos[1], 0.0);
		draw1(part.value);
		glPopMatrix();
	}
}

void ParticleSystem::draw1(double value)
{
	long const q = 16;
	double const dphi = 2.0 * M_PI / q;
	glBegin(GL_TRIANGLE_FAN);
	colorize(value);
	glVertex2d(0.0, 0.0);
	glColor4f(0.0, 0.0, 0.0, 1.0);
	for(long k = 0; k != q; ++k)
	{
		double r = 2.4;
		double phi = dphi * k;
		glVertex2d(r * std::cos(phi), r * std::sin(phi));
		glVertex2d(r * std::cos(phi + dphi), r * std::sin(phi + dphi));
	}
	glEnd();
}

void ParticleSystem::colorize(double value)
{
	double y = std::atan(value) / M_PI;
	glColor4f(y, y, y, 1.0);
}

Explosion::Explosion(World *world, PointState const &base, double power) :
	ParticleSystem(world)
{
	auto seed = std::time(nullptr);
	static std::ranlux24 gen(seed);
	static std::uniform_real_distribution<double> phi(-M_PI, M_PI);
	static std::uniform_real_distribution<double> rad(0.0, 1.0);
// 	static std::lognormal_distribution<double> vel(3.0, 0.3);
	static std::uniform_real_distribution<double> vel(0.0, 30.0);
	while(power > 0)
	{
		Particle p;
		double a = phi(gen);
		double r = rad(gen);

		double b = phi(gen);
		double v = vel(gen);

		p.value = 2.5;//30.0 / v;
		p.pos = { r * std::cos(a), r * std::sin(a) };
		p.vel = { v * std::cos(b), v * std::sin(b) };
		particles.push_back(p);
		power -= 1;
	}
}
