#pragma once
#include <list>
#include <Eigen/Core>
#include "manifold.hxx"

class World;

struct BodyTemp
{
	Eigen::Vector2d force;
	double rforce;
};

struct BodyDesc
{
	double radius;
	double mass;
	double rinertia;
};

struct CollisionTestResult
{
	Eigen::Vector2d collision_direction;
// 	Eigen::Vector2d sliding_direction;
	double distance;
	double collision_speed;
// 	double sliding_speed;
};

struct Body:
	BodyState,
	BodyTemp,
	BodyDesc
{
	World *world;
	void prepare();
	void move();
	CollisionTestResult testCollision(Body const *with, bool full_eval = false) const;
};
