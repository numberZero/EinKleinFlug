#pragma once
#include <set>
#include "manifold.hxx"

class Ship;
class ParticleSystem;

struct World
{
	SquareKleinBottle manifold;
	std::set<Ship *> ships;
	std::set<ParticleSystem *> particles;
	double t = 0.0;
	double dt;

	World(double size = 50.0);

	void prepare(double step);
	void collide();
	void cleanup();
	void move();

	void draw(Ship const *base);
};
