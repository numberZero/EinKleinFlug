#include "body.hxx"
#include "world.hxx"

Body::Body(World *world) :
	world(world)
{
}

void Body::prepare()
{
	force.setZero();
	rforce = 0;
}

void Body::move()
{
	if(mirror)
		rforce = -rforce;
	vel += world->dt * force / mass;
	rvel += world->dt * rforce / rinertia;
	world->manifold.stepState(*static_cast<BodyState *>(this), world->dt);
}

CollisionTestResult Body::testCollision(Body const *with, bool full_eval) const
{
	CollisionTestResult result;
	BodyState state2 = *with;
	world->manifold.remap(*this, state2);

	Vector2 dir = state2.pos - pos;
	Float dist = dir.norm();
	result.distance = dist - (radius + with->radius);
	if(!full_eval && result.distance >= 0)
		return result;
	result.collision_direction = dir / dist;
	Float u1 = result.collision_direction.dot(vel);
	Float u2 = result.collision_direction.dot(state2.vel);
	result.collision_speed = u1 - u2;
	return result;
}
