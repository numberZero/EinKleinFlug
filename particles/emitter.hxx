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
	Float energy = 0.0;
	PRNG generator;

public:
	ParticleEmitter(BodyState &base, std::shared_ptr<Matrix4> colorization = nullptr);
	BodyState const &base;
	Float particle_size;
	Vector2 base_position;
	Vector2 base_velocity;
	Float base_life;
	Float base_value;
	Float spread_position = 0.0;
	Float spread_velocity = 0.0;
	Float spread_life = 0.0;

	std::shared_ptr<Matrix4> const colorization;
	Float power = 0.0; ///< value / second

	void step() override;
};
