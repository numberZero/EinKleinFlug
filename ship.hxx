#pragma once
#include <list>
#include <set>
#include "body.hxx"

class Jet;

struct Ship: Body
{
private:
	void draw_info();
	void draw_model();

public:
	Ship(World *world, Float hp = 10.0, Float armor = 7.0);

	Float const recharge_rate = 0.5;
	Float const max_hp;
	Jet *jets[4];
	Float hp;
	Float armor;

	bool viable() const;
	void die();

	void move() override;
	void draw(BodyState const *base);
};
