#pragma once
#include <list>
#include <set>
#include "body.hxx"

struct Ship: Body
{
	Ship(World *world, double hp = 100.0, double armor = 7.0);

	double max_hp;
	double hp;
	double armor;
};
