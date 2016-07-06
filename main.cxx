#include <ctime>
#include <iostream>
#include <list>
#include <random>
#include <Eigen/Core>
#include <GL/gl.h>
#include <SDL.h>
#include "obj.hxx"

SDL_Window *window;
static SDL_GLContext context;
static long t_base;

GameWorld world;
GameObject *me;

void init()
{
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<double> pos(-50.0, 50.0);
	me = new GameObject(&world);
	me->mirror = false;
	me->pos = { 0.0, 0.0 };
	me->vel = { 3.0, 5.0 };
	me->rpos = 0.0;
	me->rvel = 0.1;
	me->radius = 4.0;
	me->mass = 5000.0;
	me->rinertia = 5000.0;
	for(int k = 0; k != 24; ++k)
	{
		GameObject *obj = new GameObject(&world);
		obj->mirror = false;
		obj->pos = { pos(gen), pos(gen) };
		obj->vel = { 0.0, 0.0 };
		obj->rpos = 0.0;
		obj->rvel = 0.0;
		obj->radius = 4.0;
		obj->mass = 5000.0;
		obj->rinertia = 5000.0;
	}
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
	world.gc();
	world.move();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glScalef(scale, scale, scale);
	glTranslated(0, -10.0, 0.0);

// load absolute coordinate system
	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0, 0.3);

	for(GameObject *object: world.objects)
	{
		glPushMatrix();
		BodyState state = *object;
		world.manifold.relativize(*me, state);
		glTranslated(state.pos[0], state.pos[1], 0.0);

		double hpp = object->hp / object->max_hp;
		glLineWidth(0.5);
		glColor4f(2.0 * (1.0 - hpp), 2.0 * hpp, 0.0, 1.0);
		glBegin(GL_QUADS);
		glVertex2d(-3.0, -4.5);
		glVertex2d(-3.0, -5.0);
		glVertex2d(-3.0 + 6.0 * hpp, -5.0);
		glVertex2d(-3.0 + 6.0 * hpp, -4.5);
		glEnd();

		glColor4f(0.0, 0.2, 0.0, 1.0);
		glBegin(GL_LINE_LOOP);
		glVertex2d(-3.0, -4.5);
		glVertex2d(-3.0, -5.0);
		glVertex2d(3.0, -5.0);
		glVertex2d(3.0, -4.5);
		glEnd();

		if(state.mirror)
			glScaled(-1.0, 1.0, 1.0);
		glRotatef(180.0 / M_PI * state.rpos, 0.0, 0.0, 1.0);

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
		glColor4f(0.0, 1.0, 0.8 + 0.2 * std::sin(5.0 * t), 0.5);
		glBegin(GL_LINE_LOOP);
		for(long k = 0; k != 32; ++k)
		{
			double r = object->radius;
			double phi = M_PI / 16.0 * k;
			glVertex2d(r * std::cos(phi), r * std::sin(phi));
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
