#include "phys.hxx"
#include <iostream>

void Body::destroy()
{
}

void Body::die()
{
	alive = false;
}

void Body::collided(Body *with, Eigen::Vector2d momentum)
{
	vel += momentum / mass;
}

World::World(double size):
	manifold{size}
{
}

void World::prepare(double step)
{
	dt = step;
	for(Body *body: bodies)
	{
		body->force.setZero();
		body->rforce = 0;
	}
}

void World::collide()
{
	static double const eps = 0.1;
	for(auto iter1 = bodies.begin(); iter1 != bodies.end(); )
	{
		Body *body1 = *iter1;
		for(auto iter2 = ++iter1; iter2 != bodies.end(); ++iter2)
		{ // thus we iterate over all unordered pairs
			Body *body2 = *iter2;
			BodyState state2 = *body2;
			manifold.remap(*body1, state2);
			Eigen::Vector2d dir = state2.pos - body1->pos;
			double dist = dir.norm();
			if(dist >= body1->radius + body2->radius)
				continue;
		// collision detected!
			*(BodyState *)body2 = state2;
		// body1 and body2 are in the same map now
			dir /= dist;
			double u1 = dir.dot(body1->vel);
			double u2 = dir.dot(body2->vel);
			if(u2 - u1 > eps) // already going away
				continue;
			if(u2 - u1 > -eps)
			{ // intersected
				body1->die();
				body2->die();
				continue;
			}
			double v1 = ((body1->mass - body2->mass) * u1 + 2 * body2->mass * u2) / (body1->mass + body2->mass);
			double v2 = ((body2->mass - body1->mass) * u2 + 2 * body1->mass * u1) / (body1->mass + body2->mass);
			body1->collided(body2, body1->mass * (v1 - u1) * dir);
			body2->collided(body1, body2->mass * (v2 - u2) * dir);
		}
	}
}

void World::gc()
{
	for(auto iter1 = bodies.begin(); iter1 != bodies.end(); )
	{
		Body *body1 = *iter1;
		if(!body1->alive)
		{
			bodies.erase(iter1++);
			body1->destroy();
		}
		else
			++iter1;
	}
}

void World::move()
{
	for(Body *body: bodies)
	{
		body->vel += dt * body->force / body->mass;
		body->rvel += dt * body->rforce / body->rinertia;
		manifold.stepState(*(BodyState*)body, dt);
	}
}
