#pragma once
#include <list>
#include <memory>
#include <set>
#include "body.hxx"

class Jet;
class Beam;

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
	std::shared_ptr<Jet> jets[4];
	std::shared_ptr<Beam> beams[2];

	static std::shared_ptr<Ship> create(World *world);
	static std::shared_ptr<Ship> create(World *world, Id id);
	bool viable() const;
	void die();
	void move() override;
	void draw(BodyState const *base);
};
