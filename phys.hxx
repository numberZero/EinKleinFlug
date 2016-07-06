#pragma once
#include <list>
#include <Eigen/Core>
#include "manifold.hxx"

struct Body: BodyState
{
	Eigen::Vector2d force;
	double rforce;

	double radius;
	double mass;
	double rinertia;
	bool alive = true;

	virtual void destroy();
	virtual void die();
	virtual void collided(Body *with, Eigen::Vector2d momentum);
};

struct World
{
	SquareKleinBottle manifold;
	std::list<Body *> bodies;
	double dt;

	World(double size = 50.0);

	void prepare(double step);
	void collide();
	void gc();
	void move();
};
