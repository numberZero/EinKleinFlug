#pragma once
#include "base.hxx"

class Ship;

struct Explosion: ParticleSystem
{
	Float const base_vel;
	Float const base_life;
	Explosion(World *world, PointState const &base, Float power = 200.0);
	Color getColor(Particle const &particle) override;
};

struct Jet: ParticleSystem
{
private:
	Float const full_thrust;
	Float const base_vel;
	Float const base_life;
	Float const pos_spread;
	Float const vel_spread;
	Float const size_fullpower;
	Float const particle_energy;
	Vector2 const part_vel;
	Float energy = 0.0;

public:
	Ship const *ship;
	Vector2 const pos;
	Vector2 const thrust;
	Float power = 0.0;

	Jet(Ship *ship, Vector2 shift, Vector2 thrust);
	bool viable() const override;
	void die();
	void move(Float dt) override;
	Color getColor(Particle const &particle) override;
};

struct Beam: ParticleSystem
{
private:
	Float const base_vel;
	Float const base_life;
	Float const pos_spread;
	Float const vel_spread;
	Float const size_fullpower;
	Float const particle_energy;
	Float energy = 0.0;

public:
	Ship const *ship;
	Float const power;
	Vector2 const vel;
	Vector2 const pos;
	bool shots = false;

	Beam(Ship *ship, Vector2 shift, Vector2 vel, Float power, Float range);
	bool viable() const override;
	void die();
	void move(Float dt) override;
	Color getColor(Particle const &particle) override;
};
