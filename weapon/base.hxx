#pragma once
#include <map>
#include "body.hxx"
#include "control.hxx"
#include "manifold.hxx"
#include "world.hxx"

class CMountable;
class CObject;

struct SMountPos
{
	Vector2 shift;
	Float rotation;
};

class CMountable:
	public CEntity
{
	friend class CObject;

protected:
	std::weak_ptr<CObject> base;
	SMountPos const *mount;
	BodyState state;

	CMountable();
	void step() override;
	virtual void afterMount();

public:
};

class CObject:
	public std::enable_shared_from_this<CObject>,
	public Body
{
private:
	std::map<std::shared_ptr<CMountable>, SMountPos> mounted;

protected:
	CObject(World *world);

public:
	void mount(std::shared_ptr<CMountable> object, SMountPos const &pos);
	void dismount(std::shared_ptr<CMountable> object);
};
