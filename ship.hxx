#pragma once
#include <list>
#include <memory>
#include <set>
#include "body.hxx"

class Jet;

struct Ship: Body
{
private:
	void draw_info();
	void draw_model();
	Ship(World *world);

public:
	static std::shared_ptr<Ship> create(World *world, Float hp = 10.0, Float armor = 7.0);

	Float recharge_rate = 0.5;
	Float max_hp;
	Float hp;
	Float armor;
	std::shared_ptr<Jet> jets[4];

	bool viable() const;
	void die();

	void move() override;
	void draw(BodyState const *base);
};
