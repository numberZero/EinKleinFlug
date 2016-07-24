#pragma once
#include "control.hxx"

class CMachineGun:
	private CWeaponController1,
	private IPulseWeaponControl
{
private:
	unsigned const ammo_max;
	double const reload_time;

	unsigned ammo;
	double reload;

// CEntity (CWeaponController1)
	void step() override;

// IPulseWeaponControl
	bool canFire() const override;
	bool doFire() override;

public:
	CMachineGun(unsigned ammo_max, double rate_of_fire);

	unsigned getAmmo() const;
	unsigned setAmmo(unsigned value);

	using CWeaponController1::CEntity;
	using CWeaponController1::IWeaponControl;
};
