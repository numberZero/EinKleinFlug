#pragma once
#include <list>
#include <memory>
#include "manifold.hxx"

class World;

struct Particle: PointState
{
	Float life;
	Float value;
};

class ParticleSystem
{
private:
#ifdef USE_SHADERS
	unsigned vertex_array;
	unsigned vertex_buffer;
#endif

public:
	World *const world;
	Float const particle_size;
	std::list<Particle> particles;
	std::shared_ptr<Matrix4 const> colorization;

	ParticleSystem(World *world, Float particle_size);
	virtual ~ParticleSystem();
	virtual bool viable() const;
	virtual void move();

	void draw(BodyState const *base);
	virtual Color getColor(Particle const &particle);
};
