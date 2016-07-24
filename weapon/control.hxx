#pragma once
#include "world.hxx"

/// high-level control interface
class IWeaponControl
{
public:
	virtual bool getTriggerState() const = 0;
	virtual bool setTriggerState(bool state) = 0; ///< \returns new state (not success/failure!)
	virtual bool getAutofireState() const = 0;
	virtual bool setAutofireState(bool state) = 0; ///< \returns new state (not success/failure!)
};

/// basic control helper
class CWeaponControl:
	public IWeaponControl
{
protected:
	bool autofire = false;
	bool trigger = false;

	virtual bool onTriggerPush() = 0;

public:
	bool getTriggerState() const override;
	bool setTriggerState(bool state) override;
	bool getAutofireState() const override;
	bool setAutofireState(bool state) override;
};

/// low-level control (1)
class IPulseWeaponControl
{
public:
	virtual bool canFire() const = 0;
	virtual bool doFire() = 0;
};

/// low-level control (2)
class IContinuousWeaponControl
{
public:
	virtual bool getState() const = 0;
	virtual bool setState(bool value) = 0;
};

/// control helper (1)
class CWeaponController1:
	public CEntity,
	public CWeaponControl
{
private:
	IPulseWeaponControl *const weapon;
	bool autofire = false;
	bool trigger = false;
	bool fired;

	bool onTriggerPush() override;

public:
	CWeaponController1(IPulseWeaponControl *weapon);
	void step() override;
};

/// control helper (2)
class CWeaponController2:
	public CEntity,
	public CWeaponControl
{
private:
	IContinuousWeaponControl *const weapon;
	unsigned const pulse_frames;
	bool autofire = false;
	bool trigger = false;
	unsigned pulse_remains = 0;

	bool onTriggerPush() override;

public:
	CWeaponController2(IContinuousWeaponControl *weapon, double pulse_duration);
	void step() override;
};
