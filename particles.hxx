#pragma once
#include <list>
#include "manifold.hxx"

class World;
class Ship;

struct Particle: PointState
{
// 	Vector2 scale;
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
	~ParticleSystem();

	virtual bool viable() const;

	virtual void move(Float dt);
	void draw(BodyState const *base);

	virtual void colorize(Particle const &p);
};

struct Explosion: ParticleSystem
{
	Float const base_vel;
	Float const base_life;
	Explosion(World *world, PointState const &base, Float power = 200.0);
	void colorize(Particle const &p) override;
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
	void colorize(Particle const &p) override;
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
	void colorize(Particle const &p) override;
};
