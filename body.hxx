#pragma once
#include <list>
#include "types.hxx"
#include "manifold.hxx"

class World;

struct BodyTemp
{
	Vector2 force;
	Float rforce;
};

struct BodyDesc
{
	Float radius;
	Float mass;
	Float rinertia;
};

struct CollisionTestResult
{
	Vector2 collision_direction;
// 	Vector2 sliding_direction;
	Float distance;
	Float collision_speed;
// 	Float sliding_speed;
};

struct Body:
	BodyState,
	BodyTemp,
	BodyDesc
{
	Body(World *world);
	World *world;
	void prepare();
	virtual void move();
	CollisionTestResult testCollision(Body const *with, bool full_eval = false) const;
};
