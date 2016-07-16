#include "ship.hxx"
#include <GL/gl.h>
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
	glLineWidth(2.0);
	glColor4f(1.0, 1.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex2d(0.0, -3.0);
	glVertex2d(0.0, 3.0);
	glVertex2d(0.0, 1.0);
	glVertex2d(2.0, 0.0);
	glVertex2d(0.0, 1.0);
	glVertex2d(-2.0, 0.0);
	glVertex2d(0.0, 0.0);
	glVertex2d(2.0, -1.0);
	glEnd();

	glLineWidth(3.0);
	glColor4f(0.0, 1.0, 0.8 + 0.2 * std::sin(5.0 * world->t), 0.5);
	glBegin(GL_LINE_LOOP);
	for(long k = 0; k != 32; ++k)
	{
		double r = radius;
		double phi = M_PI / 16.0 * k;
		glVertex2d(r * std::cos(phi), r * std::sin(phi));
	}
	glEnd();
}
