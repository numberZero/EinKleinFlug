#pragma once
#include <list>
#include <Eigen/Core>
#include "manifold.hxx"

struct Body: BodyState
{
	Eigen::Vector2d force;
	double rforce;

	double mass;
	double rinertia;
};

struct World
{
	SquareKleinBottle manifold;
	std::list<Body> bodies;
	double dt;

	World(double size = 50.0);

	void prepare(double step);
	void collide();
	void move();
};
