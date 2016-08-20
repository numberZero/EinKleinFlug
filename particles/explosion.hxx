#pragma once
#include "manifold.hxx"

class World;

class Explosion
{
public:
	static void create(World *world, PointState const &base, Float power = 200.0);
};
