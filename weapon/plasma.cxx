#include "plasma.hxx"
#include "particles/emitter.hxx"
#include "world.hxx"

CPlasmaCannon::CPlasmaCannon(double range, double energy_capacity, double output_power, double recharge_rate) :
	CMountable(),
	range(range),
	energy_max(energy_capacity),
	discharge_per_frame(output_power / World::frame_rate),
	recharge_per_frame(recharge_rate / World::frame_rate),
	energy(0.0),
	active(false),
	emitter(new ParticleEmitter(state))
{
	Float base_vel = 150.0;
	emitter->particle_size = 0.4;
	emitter->base_position = { 0, 0 };
	emitter->base_velocity = { 0, base_vel };
	emitter->base_life = range / base_vel;
	emitter->base_value = emitter->base_life / 400.0;
	emitter->spread_position = 0.1;
	emitter->spread_velocity = 0.04 * base_vel;
	emitter->spread_life = 0.1;
	Float c = 1.0 / (emitter->base_value * std::pow(emitter->base_life, 3));
	emitter->colorization <<
		0.0, 0.0, 2.5 * c, -1.5,
		0.0, 0.0, 5.0 * c, -4.0,
		0.0, 0.0, 3.5 * c, 0.0,
		0.0, 0.0, 0.0, 1.0;
}

void CPlasmaCannon::step()
{
	CMountable::step();
	energy = std::min(energy + recharge_per_frame, energy_max);
	if(active)
	{
		if(energy < discharge_per_frame)
		{
			active = false;
			return;
		}
		energy -= discharge_per_frame;
		emitter->energy = discharge_per_frame;
		emitter->step();
	}
}

void CPlasmaCannon::afterMount()
{
	CMountable::afterMount();
}

bool CPlasmaCannon::getState() const
{
	return active;
}

bool CPlasmaCannon::setState(bool value)
{
	active = value;
	return active;
}

std::shared_ptr<IWeaponControl> CPlasmaCannon::create(
	std::shared_ptr<CObject> base,
	SMountPos const &pos,
	double range,
	double energy_capacity,
	double output_power,
	double recharge_rate)
{
	std::shared_ptr<CPlasmaCannon> weapon(new CPlasmaCannon(range, energy_capacity, output_power, recharge_rate));
	std::shared_ptr<CWeaponController2> controller(new CWeaponController2(weapon, 0.15));
	base->mount(weapon, pos);
	world.entities.insert(weapon);
	world.entities.insert(controller);
	return controller;
}
