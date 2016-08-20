#pragma once
#include "base.hxx"
#include "control.hxx"

class CPlasmaCannon:
	public CMountable,
	public IContinuousWeaponControl
{
private:
	double const energy_max;
	double const discharge_per_frame;
	double const recharge_per_frame;

	double energy;
	bool active;

// CEntity (CMountable)
	void step() override;

public:
	CPlasmaCannon(double energy_capacity, double output_power, double recharge_rate);

// IContinuousWeaponControl
	bool getState() const override;
	bool setState(bool value) override;

	static std::shared_ptr<IWeaponControl> create(
		std::shared_ptr<CObject> base,
		SMountPos const &pos,
		double energy_capacity,
		double output_power,
		double recharge_rate);
};
