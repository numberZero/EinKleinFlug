#pragma once
#include "owned.hxx"

class Beam: public OwnedParticleSystem
{
private:
	Float base_vel;
	Float base_life;
	Float pos_spread;
	Float vel_spread;
	Float size_fullpower;
	Float particle_energy;
	Float energy = 0.0;
	Float power;
	Vector2 vel;
	Vector2 pos;

	using OwnedParticleSystem::OwnedParticleSystem;
	void generate(std::shared_ptr<Ship> ship) override;

public:
	bool shots = false;

	static std::shared_ptr<Beam> create(std::shared_ptr<Ship> ship, Vector2 shift, Vector2 vel, Float power, Float range);
	Color getColor(Particle const &particle) override;
};
