#pragma once
#include <memory>
#include <set>
#include "manifold.hxx"

class Ship;
class ParticleSystem;

class CObject:
	virtual public std::enable_shared_from_this<CObject> // exactly one across the whole hierarchy
{
public:
	virtual ~CObject() = default;
	virtual void step();
};

struct World
{
	SquareKleinBottle manifold;
	std::set<Ship *> ships;
	std::set<ParticleSystem *> particles;
	std::set<std::weak_ptr<CObject>> entities;
	Float t = 0.0;
	static constexpr Float const frame_rate = 50.0;
	static constexpr Float const dt = 1.0 / frame_rate;

	World(Float size = 50.0);

	void prepare();
	void collide();
	void cleanup();
	void move();

	void draw(Ship const *base);
};
