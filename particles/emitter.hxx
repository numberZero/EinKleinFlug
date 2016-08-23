#pragma once
#include <memory>
#include <random>
#include "entity.hxx"
#include "types.hxx"

struct BodyState;
struct Particle;
class ParticleSystem;

typedef std::ranlux24 PRNG;

class ParticleEmitter:
	public CEntity
{
private:
	std::weak_ptr<ParticleSystem> particle_system;
	PRNG generator;

public:
	ParticleEmitter(BodyState &base); // after-construction initialization is necessary
	BodyState const &base;
	Float particle_size;
	Vector2 base_position;
	Vector2 base_velocity;
	Float base_life;
	Float base_value;
	Float spread_position = 0.0;
	Float spread_velocity = 0.0;
	Float spread_life = 0.0;

	Matrix4 colorization;
	Float power = 0.0; ///< value / second
	Float energy = 0.0;

	void step() override;
};
