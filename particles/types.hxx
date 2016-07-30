#pragma once
#include <memory>
#include "base.hxx"

class Ship;

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

class Jet: public ParticleSystem
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

	Jet(std::shared_ptr<Ship> ship, Vector2 shift, Vector2 thrust);

public:
	std::shared_ptr<Ship const> ship;
	Vector2 const pos;
	Vector2 const thrust;
	Float power = 0.0;

	static std::shared_ptr<Jet> create(std::shared_ptr<Ship> ship, Vector2 shift, Vector2 thrust);
	bool viable() const override;
	void die();
	void move(Float dt) override;
	Color getColor(Particle const &particle) override;
};

class Beam: public ParticleSystem
{
private:
	Float const base_vel;
	Float const base_life;
	Float const pos_spread;
	Float const vel_spread;
	Float const size_fullpower;
	Float const particle_energy;
	Float energy = 0.0;

	Beam(std::shared_ptr<Ship> ship, Vector2 shift, Vector2 vel, Float power, Float range);

public:
	std::shared_ptr<Ship const> ship;
	Float const power;
	Vector2 const vel;
	Vector2 const pos;
	bool shots = false;

	static std::shared_ptr<Beam> create(std::shared_ptr<Ship> ship, Vector2 shift, Vector2 vel, Float power, Float range);
	bool viable() const override;
	void die();
	void move(Float dt) override;
	Color getColor(Particle const &particle) override;
};
