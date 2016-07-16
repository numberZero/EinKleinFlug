#include "world.hxx"
#include <GL/gl.h>
#include "particles.hxx"
#include "ship.hxx"

World::World(double size):
	manifold{size}
{
}

void World::prepare(double step)
{
	dt = step;
	t += dt;
	for(Ship *ship: ships)
		ship->prepare();
}

void World::collide()
{
	static double const eps = 0.1;
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
			double momentum = cinfo.collision_speed * ship1->mass * ship2->mass / (ship1->mass + ship2->mass);
			double energy = 0.5 * cinfo.collision_speed * momentum;
			double dv1 = -2 * cinfo.collision_speed * ship2->mass / (ship1->mass + ship2->mass);
			double dv2 = 2 * cinfo.collision_speed * ship1->mass / (ship1->mass + ship2->mass);
			ship1->vel += dv1 * cinfo.collision_direction;
			ship2->vel += dv2 * cinfo.collision_direction;
			ship1->hp -= 1e-3 * energy / ship1->armor;
			ship2->hp -= 1e-3 * energy / ship2->armor;
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
			parts->die();
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
