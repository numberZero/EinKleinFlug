#include "world.hxx"
#include <GL/gl.h>
#include "particles/types.hxx"
#include "ship.hxx"

constexpr Float const World::frame_rate;
constexpr Float const World::dt;

void CObject::step()
{
}

World::World(Float size):
	manifold{size}
{
}

void World::prepare()
{
	t += dt;
	for(Ship *ship: ships)
		ship->prepare();
}

void World::collide()
{
// ship-ship collisions
	static Float const eps = 0.1;
	for(auto iter1 = ships.begin(); iter1 != ships.end(); )
	{
		auto pship1 = iter1++;
		Ship *ship1 = *pship1;
		for(auto iter2 = iter1; iter2 != ships.end(); )
		{ // thus we iterate over all unordered pairs
			auto pship2 = iter2++;
			Ship *ship2 = *pship2;
			CollisionTestResult cinfo = ship1->testCollision(ship2);
			if(cinfo.distance >= 0)
				continue; // not collided
			if(cinfo.collision_speed < -eps)
				continue; // already going away
			if((cinfo.collision_speed < eps) && (cinfo.distance < -eps))
			{ // intersected
				ship1->hp = 0;
				ship2->hp = 0;
				break;
			}
			// collided
			manifold.remap(*ship1, *ship2);
			Float momentum = cinfo.collision_speed * ship1->mass * ship2->mass / (ship1->mass + ship2->mass);
			Float energy = 0.5 * cinfo.collision_speed * momentum;
			Float dv1 = -2 * cinfo.collision_speed * ship2->mass / (ship1->mass + ship2->mass);
			Float dv2 = 2 * cinfo.collision_speed * ship1->mass / (ship1->mass + ship2->mass);
			ship1->vel += dv1 * cinfo.collision_direction;
			ship2->vel += dv2 * cinfo.collision_direction;
			ship1->hp -= 1e-3 * energy / ship1->armor;
			ship2->hp -= 1e-3 * energy / ship2->armor;
		}
	}
// ship-particle collisions
	for(Ship *ship: ships)
	{
		for(ParticleSystem *ps: particles)
		{
			for(auto iter = ps->particles.begin(); iter != ps->particles.end(); )
			{
				auto ppart = iter++;
				Particle &part = *ppart;
				PointState pt(part);
				manifold.remap(*ship, pt);
				Vector2 relpos = pt.pos - ship->pos;
				if(relpos.norm() < ship->radius)
				{
					Vector2 relvel = pt.vel - ship->vel;
					if(relvel.dot(relpos) > 0) // moves away
						continue;
					ship->hp -= part.value;
					ps->particles.erase(ppart);
				}
			}
		}
	}
}

void World::cleanup()
{
	for(auto iter = ships.begin(); iter != ships.end(); )
	{
		auto pship = iter++;
		Ship *ship = *pship;
		if(!ship->viable())
		{
			ships.erase(pship);
			ship->die();
			delete ship;
		}
	}
	for(auto iter = particles.begin(); iter != particles.end(); )
	{
		auto pparts = iter++;
		ParticleSystem *parts = *pparts;
		if(!parts->viable())
		{
			particles.erase(pparts);
			delete parts;
		}
	}
}

void World::move()
{
	for(Ship *ship: ships)
		ship->move();
	for(ParticleSystem *parts: particles)
		parts->move(dt);
	for(auto iter = entities.begin(); iter != entities.end(); )
	{
		auto pentity = iter++;
		std::shared_ptr<CObject> entity(pentity->lock());
		if(entity)
			entity->step();
		else
			entities.erase(pentity);
	}
}

void World::draw(Ship const *base)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for(ParticleSystem *parts: particles)
		parts->draw(base);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(Ship *ship: ships)
		ship->draw(base);
}
