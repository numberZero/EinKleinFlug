#pragma once
#include <list>
#include "manifold.hxx"

class World;

struct Particle: PointState
{
// 	Eigen::Vector2d scale;
	double life;
	double value;
};

struct ParticleSystem
{
	World *world;
	std::list<Particle> particles;

	ParticleSystem(World *world);

	void move(double dt);
	void draw(BodyState const *base);

	virtual void draw1(Particle const &p);
	virtual void colorize(Particle const &p);
};

struct Explosion: ParticleSystem
{
	double const base_vel = 10.0;
	double const base_life = 2.5;
	Explosion(World *world, PointState const &base, double power);
	void colorize(Particle const &p) override;
};
