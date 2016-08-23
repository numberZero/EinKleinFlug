#include "base.hxx"

CMountable::CMountable()
{
}

void CMountable::step()
{
	CEntity::step();
	std::shared_ptr<CObject> object(base);
	state.pos = mount->shift;
	state.vel = {0, 0};
	state.rpos = mount->rotation;
	state.rvel = 0;
	state.mirror = false;
	world.manifold.absolutize((Body)*object, state);
}

void CMountable::afterMount()
{
}

CObject::CObject(World *world) :
	Body(world)
{
}

void CObject::mount(std::shared_ptr<CMountable> object, SMountPos const &pos)
{
	if(!object->base.expired())
		throw std::logic_error("Attempt to mount an object that is already mounted");
	auto pmount = mounted.emplace(object, pos);
	object->base = shared_from_this();
	object->mount = &pmount.first->second;
}

void CObject::dismount(std::shared_ptr<CMountable> object)
{
	if(1 != mounted.erase(object))
		throw std::logic_error("Attempt to dismount a non-mounted object");
	object->base.reset();
	object->mount = nullptr;
}
