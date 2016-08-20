#pragma once
#include "base.hxx"
#include "control.hxx"

class CMachineGun:
	public CMountable,
	public IPulseWeaponControl
{
private:
	unsigned const ammo_max;
	double const reload_time;

	unsigned ammo;
	double reload;

// CEntity (CMountable)
	void step() override;

public:
	CMachineGun(unsigned ammo_max, double rate_of_fire);

// IPulseWeaponControl
	bool canFire() const override;
	bool doFire() override;

	unsigned getAmmo() const;
	unsigned setAmmo(unsigned value);

	static std::shared_ptr<IWeaponControl> create(
		std::shared_ptr<CObject> base,
		SMountPos const &pos,
		unsigned ammo_max,
		double rate_of_fire);
};
