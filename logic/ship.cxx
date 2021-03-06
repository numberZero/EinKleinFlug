#include "ship.hxx"
#include <GL/gl.h>
#include "particles/jet.hxx"
#include "particles/explosion.hxx"
#include "visual/text.hxx"
#include "physics/world.hxx"

Id Ship::last_id = 0;

Ship::Ship(World *world, Id id) :
	Body(world),
	id(id)
{
}

std::shared_ptr<Ship> Ship::create(World *world)
{
	return create(world, ++last_id);
}

std::shared_ptr<Ship> Ship::create(World *world, Id id)
{
	std::shared_ptr<Ship> ship(new Ship(world, id));
	world->ships.emplace(ship);
	ship->hp = 10.0;
	ship->max_hp = 10.0;
	ship->armor = 7.0;
	ship->jets[0].reset(new Jet(*ship, {-2.0, -1.7}, {0.0, +75000.0}));
	ship->jets[1].reset(new Jet(*ship, {+2.0, -1.7}, {0.0, +75000.0}));
	ship->jets[2].reset(new Jet(*ship, {-2.0, +1.7}, {0.0, -75000.0}));
	ship->jets[3].reset(new Jet(*ship, {+2.0, +1.7}, {0.0, -75000.0}));
	return ship;
}

bool Ship::viable() const
{
	return hp > 0;
}

void Ship::die()
{
	Explosion::create(world, *this, 200.0);
}

void Ship::move()
{
	Matrix2 rot = world->manifold.absolutizationMatrix(*this);
	for(int k = 0; k != 4; ++k)
	{
		Vector2 shift = rot * jets[k]->getPos();
		Vector2 thrust = rot * jets[k]->getThrust();
		force += thrust;
		rforce += shift[0] * thrust[1] - shift[1] * thrust[0];
	}
	Body::move();
	hp += recharge_rate * world->dt;
	if(hp >= max_hp)
		hp = max_hp;
}

void Ship::draw(BodyState const *base)
{
	glPushMatrix();
	BodyState state = *this;
	world->manifold.relativize(*base, state);
	glTranslated(state.pos[0], state.pos[1], 0.0);
	draw_info();
	glRotated(180.0 / M_PI * state.rpos, 0.0, 0.0, 1.0);
	if(state.mirror)
		glScaled(-1.0, 1.0, 1.0);
	draw_model();
	glPopMatrix();
}

void Ship::draw_info()
{
	Float hpp = hp / max_hp;
	glColor4f(2.0 * (1.0 - hpp), 2.0 * hpp, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex2d(-3.0, -4.5);
	glVertex2d(-3.0, -5.0);
	glVertex2d(-3.0 + 6.0 * hpp, -5.0);
	glVertex2d(-3.0 + 6.0 * hpp, -4.5);
	glEnd();

	glLineWidth(0.5);
	glColor4f(0.0, 0.2, 0.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-3.0, -4.5);
	glVertex2d(-3.0, -5.0);
	glVertex2d(3.0, -5.0);
	glVertex2d(3.0, -4.5);
	glEnd();

	glColor4f(1.0, 1.0, 1.0, 0.7);
	vglTextOutF(-3.0, -6.0, 1.0, 1.0, "HP: %.1f (%.0f %%)", hp, 100.0 * hpp);
}

void Ship::draw_model()
{
	static long const q = 32;
	glBegin(GL_QUADS);
	glColor4f(0.0, 0.0, 0.3, 1.0);
	glVertex2d(-3.0, 0.0);
	glVertex2d(-3.0, 1.0);
	glVertex2d(0.0, 0.0);
	glVertex2d(0.0, -2.0);
	glColor4f(0.3, 0.0, 0.0, 1.0);
	glVertex2d(3.0, 0.0);
	glVertex2d(3.0, 1.0);
	glVertex2d(0.0, 0.0);
	glVertex2d(0.0, -2.0);
	glColor4f(0.3, 0.3, 0.3, 1.0);
	glVertex2d(-2.3, -1.8);
	glVertex2d(-2.3, 1.8);
	glVertex2d(-1.7, 1.8);
	glVertex2d(-1.7, -1.8);
	glVertex2d(2.3, -1.8);
	glVertex2d(2.3, 1.8);
	glVertex2d(1.7, 1.8);
	glVertex2d(1.7, -1.8);
	glColor4f(0.0, 0.3, 0.0, 1.0);
	glVertex2d(-0.6, -2.6);
	glVertex2d(-0.6, 2.0);
	glVertex2d(0.0, 2.4);
	glVertex2d(0.0, -2.6);
	glVertex2d(0.6, -2.6);
	glVertex2d(0.6, 2.0);
	glVertex2d(0.0, 2.4);
	glVertex2d(0.0, -2.6);
	glEnd();

	Float const b = 0.8 + 0.2 * std::sin(5.0 * world->t);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBegin(GL_QUAD_STRIP);
	for(long k = 0; k != q + 1; ++k)
	{
		Float r1 = radius * 0.85;
		Float r2 = radius * 1.0;
		Float phi = 2.0 * M_PI / q * k;
		glColor4f(0.0, 1.0, b, 0.0);
		glVertex2d(r1 * std::cos(phi), r1 * std::sin(phi));
		glColor4f(0.0, 1.0, b, 0.7);
		glVertex2d(r2 * std::cos(phi), r2 * std::sin(phi));
	}
	glEnd();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
