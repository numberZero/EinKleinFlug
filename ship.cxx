#include "ship.hxx"
#include "world.hxx"

Ship::Ship(World *world, double hp, double armor) :
	max_hp(hp),
	hp(hp),
	armor(armor)
{
	this->world = world;
	world->ships.push_back(this);
}
