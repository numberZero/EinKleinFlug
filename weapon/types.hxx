#pragma once
#include "base.hxx"
#include "control.hxx"

class CMachineGun:
	public CMountable,
	public CWeaponController1,
	private IPulseWeaponControl // can’t be public because used by CWeaponController1
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
	CMachineGun(SMountInfo const &mount, unsigned ammo_max, double rate_of_fire);

	unsigned getAmmo() const;
	unsigned setAmmo(unsigned value);
};

class CPlasmaCannon:
	public CMountable,
	public CWeaponController2,
	private IContinuousWeaponControl
{
private:
	double const energy_max;
	double const discharge_per_frame;
	double const recharge_per_frame;

	double energy;
	bool active;

// CEntity (CWeaponController2)
	void step() override;

// IContinuousWeaponControl
	bool getState() const override;
	bool setState(bool value) override;

public:
	CPlasmaCannon(SMountInfo const &mount, double energy_capacity, double output_power, double recharge_rate);
};
