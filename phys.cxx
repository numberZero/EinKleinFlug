#include "phys.hxx"

World::World(double size):
	manifold{size}
{
}

void World::prepare(double step)
{
	dt = step;
	for(Body &body: bodies)
	{
		body.force.setZero();
		body.rforce = 0;
	}
}

void World::collide()
{

}

void World::move()
{
	for(Body &body: bodies)
	{
		body.vel += dt * body.force / body.mass;
		body.rvel += dt * body.rforce / body.rinertia;
		manifold.stepState(body, dt);
	}
}
