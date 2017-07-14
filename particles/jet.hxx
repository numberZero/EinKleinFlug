#pragma once
#include <memory>
#include "common/types.hxx"

class ParticleEmitter;
class Ship;

class Jet
{
private:
	Ship const &ship;
	std::shared_ptr<ParticleEmitter> emitter;
	Float power;
	Vector2 pos;
	Vector2 thrust;

public:
	Jet(Ship &ship, Vector2 shift, Vector2 thrust);

	Float getPower() const;
	void setPower(Float value);
	Vector2 getPos() const { return pos; }
	Vector2 getThrust() const { return power * thrust; }
};
