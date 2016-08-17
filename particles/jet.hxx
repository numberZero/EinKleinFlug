#pragma once
#include <memory>
#include "types.hxx"

class ParticleEmitter;
class Ship;

class Jet
{
private:
	Ship const &ship;
	std::shared_ptr<ParticleEmitter> emitter;
	Float power;
	Vector2 pos;
	Vector2 thrust;

public:
	Jet(Ship &ship, Vector2 shift, Vector2 thrust);

	Float getPower() const;
	void setPower(Float value);
	Vector2 getPos() const { return pos; }
	Vector2 getThrust() const { return power * thrust; }
};
/*
class Jet: public OwnedParticleSystem
{
private:
	Float full_thrust;
	Float base_vel;
	Float base_life;
	Float pos_spread;
	Float vel_spread;
	Float size_fullpower;
	Float particle_energy;
	Vector2 part_vel;
	Vector2 pos;
	Vector2 thrust;
	Float energy = 0.0;

	using OwnedParticleSystem::OwnedParticleSystem;
	void generate(std::shared_ptr<Ship> ship) override;

public:
	Float power = 0.0;

	static std::shared_ptr<Jet> create(std::shared_ptr<Ship> ship, Vector2 shift, Vector2 thrust);
	Color getColor(Particle const &particle) override;

	Vector2 getPos() { return pos; }
	Vector2 getThrust() { return thrust; }
};


*/
