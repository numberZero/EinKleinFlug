#include "world.hxx"
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
				ships.erase(pship1);
				if(iter1 == pship2)
					++iter1;
				ships.erase(pship2);
				break;
			}
			// collided
			manifold.remap(*ship1, *ship2);
 			double dv1 = -2 * cinfo.collision_speed * ship2->mass / (ship1->mass + ship2->mass);
 			double dv2 = 2 * cinfo.collision_speed * ship1->mass / (ship1->mass + ship2->mass);
			ship1->vel += dv1 * cinfo.collision_direction;
			ship2->vel += dv2 * cinfo.collision_direction;
		}
	}
}

void World::move()
{
	for(Ship *ship: ships)
		ship->move();
}
