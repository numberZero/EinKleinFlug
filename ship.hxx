#pragma once
#include <list>
#include <set>
#include "body.hxx"

struct Ship: Body
{
private:
	void draw_info();
	void draw_model();

public:
	Ship(World *world, double hp = 100.0, double armor = 7.0);

	double const max_hp;
	double hp;
	double armor;

	void draw(BodyState const *base);
};
