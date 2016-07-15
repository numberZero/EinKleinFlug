#pragma once
#include <list>
#include "manifold.hxx"

class Ship;
class ParticleSystem;

struct World
{
	SquareKleinBottle manifold;
 	std::list<Ship *> ships;
 	std::list<ParticleSystem *> particles;
	double dt;

	World(double size = 50.0);

	void prepare(double step);
	void collide();
	void move();
};
