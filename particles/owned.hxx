#pragma once
#include "base.hxx"
#include <memory>

class Ship;

class OwnedParticleSystem: public ParticleSystem
{
private:
	std::weak_ptr<Ship> ship;

protected:
	OwnedParticleSystem(std::shared_ptr<Ship> ship, Float particle_size);
	void detach();
	virtual void generate(std::shared_ptr<Ship> ship) = 0;

public:
	bool viable() const override;
	void move() override;
};
