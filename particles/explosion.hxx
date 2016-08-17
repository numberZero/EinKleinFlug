#pragma once
#include "system.hxx"

class Explosion: public ParticleSystem
{
private:
	Explosion(World *world, PointState const &base, Float power = 200.0);

public:
	Float const base_vel;
	Float const base_life;

	static void create(World *world, PointState const &base, Float power = 200.0);
	Color getColor(Particle const &particle) override;
};
