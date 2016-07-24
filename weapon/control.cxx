#include "control.hxx"

bool CWeaponControl::getTriggerState() const
{
	return trigger;
}

bool CWeaponControl::setTriggerState(bool state)
{
	bool push = state && !trigger;
	trigger = state;
	if(push)
		onTriggerPush();
}

bool CWeaponControl::getAutofireState() const
{
	return autofire;
}

bool CWeaponControl::setAutofireState(bool state)
{
	autofire = state;
	return autofire;
}

CWeaponController1::CWeaponController1(IPulseWeaponControl *weapon) :
	weapon(weapon)
{
}

void CWeaponController1::step()
{
	if(trigger && (autofire || !fired) && weapon->canFire())
	{
		weapon->doFire();
		fired = true;
	}
}

bool CWeaponController1::onTriggerPush()
{
	fired = false;
}

CWeaponController2::CWeaponController2(IContinuousWeaponControl *weapon, double pulse_duration) :
	weapon(weapon),
	pulse_frames(pulse_duration * World::frame_rate)
{
}

void CWeaponController2::step()
{
	if(trigger && (autofire || pulse_remains > 0))
	{
		if(!weapon->setState(true))
			pulse_remains = 0;
		if(pulse_remains > 0)
			--pulse_remains;
	}
	else
		weapon->setState(false);
}

bool CWeaponController2::onTriggerPush()
{
	pulse_remains = pulse_frames;
}
