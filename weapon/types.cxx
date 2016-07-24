#include "types.hxx"
#include <iostream>

CMachineGun::CMachineGun(unsigned ammo_max, double rate_of_fire) :
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
