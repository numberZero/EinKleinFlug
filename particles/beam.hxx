#pragma once
#include <memory>
#include "types.hxx"

class ParticleEmitter;
class Ship;

class Beam
{
private:
	Ship const &ship;
	std::shared_ptr<ParticleEmitter> emitter;
	Float power;
	bool shots = false;

public:
	bool doesShot() const;
	void setShot(bool value);

	Beam(Ship &ship, Vector2 shift, Vector2 vel, Float power, Float range);
};
