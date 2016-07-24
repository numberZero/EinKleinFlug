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
	Float t = 0.0;
	Float dt;

	World(Float size = 50.0);

	void prepare(Float step);
	void collide();
	void cleanup();
	void move();

	void draw(Ship const *base);
};
