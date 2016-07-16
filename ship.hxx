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
	Ship(World *world, double hp = 10.0, double armor = 7.0);

	double const recharge_rate = 0.5;
	double const max_hp;
	double hp;
	double armor;

	bool viable() const;
	void die();

	void move() override;
	void draw(BodyState const *base);
};
