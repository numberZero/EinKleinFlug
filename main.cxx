#include <ctime>
#include <iostream>
#include <list>
#include <random>
#include <Eigen/Core>
#include <GL/gl.h>
#include <SDL.h>
#include "phys.hxx"

SDL_Window *window;
static SDL_GLContext context;
static long t_base;

World world;
Body me;
std::list<Body> they;

void init()
{
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<double> pos{-50.0, 50.0};
	me.mirror = false;
	me.pos = { 0.0, 0.0 };
	me.vel = { 3.0, 5.0 };
	me.rpos = 0.0;
	me.rvel = 0.0;
	me.radius = 4.0;
	me.mass = 5000.0;
	me.rinertia = 5000.0;
	for(int k = 0; k != 24; ++k)
	{
		Body body;
		body.mirror = false;
		body.pos = { pos(gen), pos(gen) };
		body.vel = { 0.0, 0.0 };
		body.rpos = 0.0;
		body.rvel = 0.0;
		body.radius = 4.0;
		body.mass = 5000.0;
		body.rinertia = 5000.0;
		they.push_back(body);
	}
	world.bodies.push_back(&me);
	for(Body &body: they)
		world.bodies.push_back(&body);
}

void step()
{
	static double const scale = 10.0;
	long const t_now = SDL_GetTicks();
	double const t = 0.001 * t_now;
	double const dt = 0.001 * (t_now - t_base);
	t_base = t_now;

	world.prepare(dt);
	world.collide();
	world.move();

	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glScalef(scale, scale, scale);
	glTranslated(0, -10.0, 0.0);

// load absolute coordinate system
	glPushMatrix();
	glRotatef(90.0 - 180.0 / M_PI * me.rpos, 0.0, 0.0, 1.0);
	glColor4f(1.0, 1.0, 1.0, 0.3);
	if(me.mirror)
		glScaled(-1.0, 1.0, 1.0);
	glTranslated(-me.pos[0], -me.pos[1], 0.0);

	for(Body *body: world.bodies)
	{
		glPushMatrix();
		BodyState state = *body;
		world.manifold.remap(me, state);
		glTranslated(state.pos[0], state.pos[1], 0.0);
		if(state.mirror)
			glScaled(-1.0, 1.0, 1.0);
		glRotatef(180.0 / M_PI * state.rpos, 0.0, 0.0, 1.0);

		glLineWidth(2.0);
		glColor4f(1.0, 1.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex2d(-3.0, 0.0);
		glVertex2d(3.0, 0.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, 2.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, -2.0);
		glVertex2d(0.0, 0.0);
		glVertex2d(-1.0, -2.0);
		glEnd();

		glLineWidth(3.0);
		glColor4f(0.0, 1.0, 0.8 + 0.2 * std::sin(5.0 * t), 0.5);
		glBegin(GL_LINE_LOOP);
		for(long k = 0; k != 32; ++k)
		{
			double phi = M_PI / 16.0 * k;
			glVertex2d(body->radius * std::cos(phi), body->radius * std::sin(phi));
		}
		glEnd();

		glPopMatrix();
	}
	glPopMatrix(); // return to relative coordinate system

	glLineWidth(1.5);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	glVertex2d(-5.0, -3.0);
	glVertex2d(-5.0, -5.0);
	glVertex2d(-3.0, -5.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(5.0, -3.0);
	glVertex2d(5.0, -5.0);
	glVertex2d(3.0, -5.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(5.0, 3.0);
	glVertex2d(5.0, 5.0);
	glVertex2d(3.0, 5.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(-5.0, 3.0);
	glVertex2d(-5.0, 5.0);
	glVertex2d(-3.0, 5.0);
	glEnd();

	glFlush();
	glFinish();
	SDL_GL_SwapWindow(window);
}

bool events()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				return false;
		}
	}
	return true;
}

void run()
{
	while(events())
		step();
}

void initSDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Ein Klein Flug", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);
	t_base = SDL_GetTicks();
}

void initGL()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH);
	
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glLineWidth(1.5);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-400, 400, -300, 300, -10, 100);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
	initSDL();
	initGL();
	init();
	run();
	SDL_Quit();
	return 0;
}
