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

struct SMountInfo
{
	CObject *base;
	SMountPos pos;
	BodyDesc desc;
};

struct SMount
{
	CMountable *object;
	Vector2 shift;
	Float rotation;
};

class CMountable:
	public CEntity,
	public BodyDesc
{
protected:
	CObject *base;
	SMountPos const *mount;

	CMountable(SMountInfo const &mount);
	CMountable(CObject *base, SMountPos const &pos, BodyDesc const &desc);

public:
};

class CObject:
	public CMountable,
	public Body
{
private:
	std::map<std::shared_ptr<CMountable>, SMountPos> mounted;

public:
	void mount(CMountable *object, SMountPos const &pos);
	void dismount(CMountable *object);
};
