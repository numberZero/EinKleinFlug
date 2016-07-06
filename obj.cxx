#include "obj.hxx"
#include <iostream>

GameObject::GameObject(GameWorld *world, double hp, double armor) :
	world(world),
	max_hp(hp),
	hp(hp),
	armor(armor)
{
	world->objects.insert(this);
	world->bodies.push_back(this);
}

void GameObject::destroy()
{
	Body::destroy();
	world->objects.erase(this);
	delete this;
}

void GameObject::collided(Body *with, Eigen::Vector2d momentum)
{
	Body::collided(with, momentum);
	double dhp = momentum.norm() * std::exp(-armor);
	hp -= dhp;
	if(hp <= 0)
		die();
}
