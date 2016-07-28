#pragma once
#include <map>
#include "body.hxx"
#include "control.hxx"
#include "manifold.hxx"
#include "world.hxx"

class CMountable;
class CEntity;

struct SMountPos
{
	Vector2 shift;
	Float rotation;
};

struct SMountInfo
{
	CEntity *base;
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
	public CObject,
	public BodyDesc
{
protected:
	CEntity *base;
	SMountPos const *mount;

	CMountable(SMountInfo const &mount);
	CMountable(CEntity *base, SMountPos const &pos, BodyDesc const &desc);

public:
};

class CEntity:
	public CMountable,
	public Body
{
private:
	std::map<std::shared_ptr<CMountable>, SMountPos> mounted;

public:
	void mount(CMountable *object, SMountPos const &pos);
	void dismount(CMountable *object);
};
