#pragma once
#include "base.hxx"
#include "control.hxx"

class ParticleEmitter;

class CPlasmaCannon:
	public CMountable,
	public IContinuousWeaponControl
{
private:
	double const range;
	double const energy_max;
	double const discharge_per_frame;
	double const recharge_per_frame;

	double energy;
	bool active;

	std::shared_ptr<ParticleEmitter> emitter;

protected:
// CMountable
	void step() override;
	void afterMount() override;

public:
	CPlasmaCannon(double range, double energy_capacity, double output_power, double recharge_rate);

// IContinuousWeaponControl
	bool getState() const override;
	bool setState(bool value) override;

	static std::shared_ptr<IWeaponControl> create(
		std::shared_ptr<CObject> base,
		SMountPos const &pos,
		double range,
		double energy_capacity,
		double output_power,
		double recharge_rate);
};
