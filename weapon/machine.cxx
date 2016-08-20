#include "machine.hxx"
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
