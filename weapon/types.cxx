#include "types.hxx"
#include <iostream>
#include "world.hxx"

CMachineGun::CMachineGun(unsigned ammo_max, double rate_of_fire) :
	CMountable(),
	ammo_max(ammo_max),
	reload_time(1.0 / rate_of_fire),
	ammo(ammo_max),
	reload(0.0)
{
}

void CMachineGun::step()
{
	if(reload > 0)
		reload -= World::dt;
}

bool CMachineGun::canFire() const
{
	return reload <= 0;
}

bool CMachineGun::doFire()
{
	if(reload > 0)
		return false;
// TODO: Generate bullet objects here
	return true;
}

unsigned CMachineGun::getAmmo() const
{
	return ammo;
}

unsigned CMachineGun::setAmmo(unsigned value)
{
	ammo = std::min(value, ammo_max);
	return ammo;
}

std::shared_ptr<IWeaponControl> CMachineGun::create(
	std::shared_ptr<CObject> base,
	SMountPos const &pos,
	unsigned ammo_max,
	double rate_of_fire)
{
	std::shared_ptr<CMachineGun> weapon(new CMachineGun(ammo_max, rate_of_fire));
	std::shared_ptr<CWeaponController1> controller(new CWeaponController1(weapon));
	base->mount(weapon, pos);
	world.entities.insert(weapon);
	world.entities.insert(controller);
	return controller;
}

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
