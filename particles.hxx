#pragma once
#include <list>
#include "manifold.hxx"

class World;

struct Particle: PointState
{
	double value;
};

struct ParticleSystem
{
	World *world;
	std::list<Particle> particles;

	ParticleSystem(World *world);

	void move(double dt);
	void draw(BodyState const *base);

	virtual void draw1(double value);
	virtual void colorize(double value);
};

struct Explosion: ParticleSystem
{
	Explosion(World *world, PointState const &base, double power);

// 	void colorize(double value) override;
};
