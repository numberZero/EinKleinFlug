#pragma once
#include <set>
#include "phys.hxx"

struct GameObject;

struct GameWorld: World
{
	std::set<GameObject *> objects;
};

struct GameObject: Body
{
	GameObject(GameWorld *world, double hp = 100.0, double armor = 7.0);

	GameWorld *world;
	double max_hp;
	double hp;
	double armor;

	void destroy() override;
	void collided(Body* with, Eigen::Vector2d momentum) override;
};
