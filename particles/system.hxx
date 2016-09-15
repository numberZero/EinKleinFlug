#pragma once
#include <list>
#include <memory>
#include "physics/manifold.hxx"

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
	Matrix4 const colorization;
	std::list<Particle> particles;

	ParticleSystem(World *world, Float particle_size, Matrix4 colorization);
	~ParticleSystem();
	bool viable() const;
	void move();

	void draw(BodyState const *base);
	Color getColor(Particle const &particle);
};
