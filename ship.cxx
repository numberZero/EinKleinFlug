#include "ship.hxx"
#include <GL/gl.h>
#include "particles.hxx"
#include "text.hxx"
#include "world.hxx"

Ship::Ship(World *world, double hp, double armor) :
	max_hp(hp),
	hp(hp),
	armor(armor)
{
	this->world = world;
	world->ships.push_back(this);
}

bool Ship::viable() const
{
	return hp > 0;
}

void Ship::die()
{
	world->particles.push_back(new Explosion(world, *this, 200.0));
}

void Ship::move()
{
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
	if(state.mirror)
		glScaled(-1.0, 1.0, 1.0);
	glRotated(180.0 / M_PI * state.rpos, 0.0, 0.0, 1.0);
	draw_model();
	glPopMatrix();
}

void Ship::draw_info()
{
	double hpp = hp / max_hp;
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

	double const b = 0.8 + 0.2 * std::sin(5.0 * world->t);
	glBegin(GL_QUAD_STRIP);
	for(long k = 0; k != q + 1; ++k)
	{
		double r1 = radius * 0.85;
		double r2 = radius * 1.0;
		double phi = 2.0 * M_PI / q * k;
		glColor4f(0.0, 1.0, b, 0.0);
		glVertex2d(r1 * std::cos(phi), r1 * std::sin(phi));
		glColor4f(0.0, 1.0, b, 0.7);
		glVertex2d(r2 * std::cos(phi), r2 * std::sin(phi));
	}
	glEnd();
}
