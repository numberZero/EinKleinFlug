#pragma once
#include <list>
#include <memory>
#include <set>
#include "body.hxx"

class Jet;

struct Ship: Body
{
private:
	static unsigned long last_id;

	Ship(World *world, unsigned long id);
	void draw_info();
	void draw_model();

public:
	unsigned long const id;
	Float recharge_rate = 0.5;
	Float max_hp;
	Float hp;
	Float armor;
	std::shared_ptr<Jet> jets[4];

	static std::shared_ptr<Ship> create(World *world);
	static std::shared_ptr<Ship> create(World *world, unsigned long const id);
	bool viable() const;
	void die();
	void move() override;
	void draw(BodyState const *base);
};
