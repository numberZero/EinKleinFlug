#pragma once
#include <list>
#include <memory>
#include <set>
#include "body.hxx"
#include "weapon/base.hxx"
#include "weapon/control.hxx"

class Jet;

struct Ship: CObject
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
	std::unique_ptr<Jet> jets[4];
	std::shared_ptr<IWeaponControl> beams[2];

	static std::shared_ptr<Ship> create(World *world);
	static std::shared_ptr<Ship> create(World *world, unsigned long const id);
	bool viable() const;
	void die();
	void move() override;
	void draw(BodyState const *base);
};
