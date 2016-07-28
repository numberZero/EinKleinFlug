#pragma once
#include <list>
#include "manifold.hxx"

class World;

struct Particle: PointState
{
	Float life;
	Float value;
};

struct ParticleSystem
{
private:
	unsigned drawlist;
	void draw1();

public:
	World *world;
	std::list<Particle> particles;
	Float const particle_size;

	ParticleSystem(World *world, Float particle_size);
	virtual ~ParticleSystem();
	virtual bool viable() const;
	virtual void move(Float dt);

	void draw(BodyState const *base);
	virtual Color getColor(Particle const &particle);
};
