#pragma once
#include <list>
#include "manifold.hxx"

class World;
class Ship;

struct Particle: PointState
{
// 	Eigen::Vector2d scale;
	double life;
	double value;
	bool left;
};

struct ParticleSystem
{
	World *world;
	Ship const *ship;
	std::list<Particle> particles;
	double const particle_size;

	ParticleSystem(Ship *base, double particle_size);
	ParticleSystem(World *world, double particle_size);

	virtual bool viable() const;
	void die();

	virtual void move(double dt);
	void draw(BodyState const *base);

	virtual void draw1(Particle const &p);
	virtual void colorize(Particle const &p);
};

struct Explosion: ParticleSystem
{
	double const base_vel;
	double const base_life;
	Explosion(World *world, PointState const &base, double power = 200.0);
	void colorize(Particle const &p) override;
};

struct Jet: ParticleSystem
{
	double const full_thrust;
	double const base_vel;
	double const base_life;
	double const pos_spread;
	double const vel_spread;
	double const size_fullpower;
	double const particle_energy;
	PointState const shift;
	double energy = 0.0;
	double power;
	Jet(Ship *ship, PointState const &shift, double full_thrust, double visual_scale = 0.1, double visual_density = 1.0);
	bool viable() const override;
	void move(double dt) override;
	void colorize(Particle const &p) override;
};
