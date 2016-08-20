#pragma once
#include <memory>
#include <set>
#include "entity.hxx"
#include "manifold.hxx"

class Ship;
class ParticleSystem;

struct World
{
	SquareKleinBottle manifold;
	std::set<std::shared_ptr<Ship>> ships;
	std::set<std::shared_ptr<ParticleSystem>> particles;
	std::set<std::weak_ptr<CEntity>, std::owner_less<std::weak_ptr<CEntity>>> entities;
	unsigned long frame = 0;
	Float t = 0.0;
	static constexpr Float const frame_rate = 50.0;
	static constexpr Float const dt = 1.0 / frame_rate;

	World(Float size = 50.0);

	void prepare();
	void collide();
	void cleanup();
	void move();

	void draw(std::shared_ptr<Ship const> base);
};

extern World world;
