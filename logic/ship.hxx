#pragma once
#include <list>
#include <memory>
#include <set>
#include "physics/body.hxx"
#include "types.hxx"

class Jet;

struct Ship: Body
{
private:
	static Id last_id;

	Ship(World *world, Id id);
	void draw_info();
	void draw_model();

public:
	Id const id;
	Float recharge_rate = 0.5;
	Float max_hp;
	Float hp;
	Float armor;
	std::unique_ptr<Jet> jets[4];

	static std::shared_ptr<Ship> create(World *world);
	static std::shared_ptr<Ship> create(World *world, Id const id);
	bool viable() const;
	void die();
	void move() override;
	void draw(BodyState const *base);
};
