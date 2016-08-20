#include "plasma.hxx"
#include "world.hxx"

CPlasmaCannon::CPlasmaCannon(double energy_capacity, double output_power, double recharge_rate) :
	CMountable(),
	energy_max(energy_capacity),
	discharge_per_frame(output_power / World::frame_rate),
	recharge_per_frame(recharge_rate / World::frame_rate),
	energy(0.0),
	active(false)
{
}

void CPlasmaCannon::step()
{
	energy = std::min(energy + recharge_per_frame, energy_max);
	if(active)
	{
		if(energy < discharge_per_frame)
		{
			active = false;
			return;
		}
		energy -= discharge_per_frame;
// TODO: Generate plasma cloud here
	}
}

bool CPlasmaCannon::getState() const
{
	return active;
}

bool CPlasmaCannon::setState(bool value)
{
	active = value;
	return active;
}

std::shared_ptr<IWeaponControl> CPlasmaCannon::create(
	std::shared_ptr<CObject> base,
	SMountPos const &pos,
	double energy_capacity,
	double output_power,
	double recharge_rate)
{
	std::shared_ptr<CPlasmaCannon> weapon(new CPlasmaCannon(energy_capacity, output_power, recharge_rate));
	std::shared_ptr<CWeaponController2> controller(new CWeaponController2(weapon, 0.3));
	base->mount(weapon, pos);
	world.entities.insert(weapon);
	world.entities.insert(controller);
	return controller;
}
