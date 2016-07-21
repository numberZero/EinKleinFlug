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
private:
	double const full_thrust;
	double const base_vel;
	double const base_life;
	double const pos_spread;
	double const vel_spread;
	double const size_fullpower;
	double const particle_energy;
	Eigen::Vector2d const part_vel;
	double energy = 0.0;

public:
	Eigen::Vector2d const pos;
	Eigen::Vector2d const thrust;
	double power = 0.0;

	Jet(Ship *ship, Eigen::Vector2d shift, Eigen::Vector2d thrust);
	bool viable() const override;
	void die();
	void move(double dt) override;
	void colorize(Particle const &p) override;
};

struct Beam: ParticleSystem
{
private:
	double const base_vel;
	double const base_life;
	double const pos_spread;
	double const vel_spread;
	double const size_fullpower;
	double const particle_energy;
	double energy = 0.0;

public:
	double const power;
	Eigen::Vector2d const vel;
	Eigen::Vector2d const pos;
	bool shots = false;

	Beam(Ship *ship, Eigen::Vector2d shift, Eigen::Vector2d vel, double power, double range);
	bool viable() const override;
	void die();
	void move(double dt) override;
	void colorize(Particle const &p) override;
};
