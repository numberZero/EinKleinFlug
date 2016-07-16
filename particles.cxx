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
	long const q = 16;
	double const dphi = 2.0 * M_PI / q;
	glBegin(GL_TRIANGLE_FAN);
	colorize(p);
	glVertex2d(0.0, 0.0);
	glColor4f(0.0, 0.0, 0.0, 1.0);
	for(long k = 0; k != q; ++k)
	{
		double r = 2.0;
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
	ParticleSystem(world),
	base_vel(std::sqrt(power / 2.0)),
	base_life(std::sqrt(power / 32.0))
{
	auto seed = std::time(nullptr);
	static std::ranlux24 gen(seed);
	static std::uniform_real_distribution<double> phi(-M_PI, M_PI);
	static std::uniform_real_distribution<double> rad(0.0, 1.0);
	static std::uniform_real_distribution<double> vel(0.0, 2.0 * base_vel);
	static std::uniform_real_distribution<double> dlife(0.9, 1.1);
	while(power > 0)
	{
		Particle p;
		double a = phi(gen);
		double r = rad(gen);

		double b = phi(gen);
		double v = vel(gen);

		p.pos = r * Eigen::Vector2d{ std::cos(a), std::sin(a) };
		p.vel = v * Eigen::Vector2d{ std::cos(b), std::sin(b) };
		p.value = 1 - 0.05 * v;
		p.life = base_life * (0.5 + 0.025 * v) * dlife(gen);
		particles.push_back(p);
		power -= p.value;
	}
}

void Explosion::colorize(Particle const &p)
{
	double v = p.value * std::pow(p.life / base_life, 1.0);
	glColor4f(5.0 * std::pow(v, 1.5), 5.0 * v - 1.0, 10.0 * v - 3.0, 1.0);
}
