#include "base.hxx"

CMountable::CMountable(SMountInfo const &mount)
{

}

CMountable::CMountable(CEntity *base, SMountPos const &pos, BodyDesc const&desc)
{

}

void CEntity::mount(CMountable *object, SMountPos const &pos)
{

}

void CEntity::dismount(CMountable *object)
{

}

/*
CCannon::CCannon(SMountInfo const &mount) :
	mount(mount)
{
	assert(mount.mount_base);
}

std::shared_ptr<IWeaponControl> CCannon::getControl() const
{
	if(!control) // possible only in fresh-constructed CCannon as shared_from_this is not available on that point yet
		control = std::dynamic_pointer_cast<IWeaponControl>(std::const_pointer_cast<CObject>(shared_from_this()));
	assert(control);
	return control;
}

void CCannon::setControl(std::shared_ptr<IWeaponControl> new_control)
{
	assert(new_control);
	control = new_control;
}

bool CCannon::resetControl()
{
	std::shared_ptr<IWeaponControl> temp = std::dynamic_pointer_cast<IWeaponControl>(shared_from_this());
	if(!temp)
		return false;
	control = temp;
	return true;
}
*/