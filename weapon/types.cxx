#include "types.hxx"
#include <iostream>

CMachineGun::CMachineGun(SMountInfo const &mount, unsigned ammo_max, double rate_of_fire) :
	CMountable(mount),
	CWeaponController1(this),
	ammo_max(ammo_max),
	reload_time(1.0 / rate_of_fire),
	ammo(ammo_max),
	reload(0.0)
{
}

void CMachineGun::step()
{
	CWeaponController1::step();
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

CPlasmaCannon::CPlasmaCannon(SMountInfo const &mount, double energy_capacity, double output_power, double recharge_rate) :
	CMountable(mount),
	CWeaponController2(this, 0.3),
	energy_max(energy_capacity),
	discharge_per_frame(output_power / World::frame_rate),
	recharge_per_frame(recharge_rate / World::frame_rate),
	energy(0.0),
	active(false)
{
}

void CPlasmaCannon::step()
{
	CWeaponController2::step();
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
