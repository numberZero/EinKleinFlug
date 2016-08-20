#include "base.hxx"

void CObject::mount(std::shared_ptr<CMountable> object, SMountPos const &pos)
{
	if(object->base)
		throw std::logic_error("Attempt to mount an object that is already mounted");
	auto pmount = mounted.emplace(object, pos);
	object->base = shared_from_this();
	object->mount = &pmount.first->second;
}

void CObject::dismount(std::shared_ptr<CMountable> object)
{
	if(1 != mounted.erase(object))
		throw std::logic_error("Attempt to dismount a non-mounted object");
	object->base = nullptr;
	object->mount = nullptr;
}
