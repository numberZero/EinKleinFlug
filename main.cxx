#include <ctime>
#include <iostream>
#include <list>
#include <random>
#include <Eigen/Core>
#include <GL/gl.h>
#include <GL/glut.h>
#include <SDL.h>
#include "particles.hxx"
#include "ship.hxx"
#include "text.hxx"
#include "world.hxx"

SDL_Window *window;
static SDL_GLContext context;
static long t_base;

World world(100.0);
Ship *me;
Explosion *e1;

void init()
{
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<double> pos(-world.manifold.radius, world.manifold.radius);
	static std::uniform_real_distribution<double> phi(-M_PI, M_PI);
	static std::uniform_real_distribution<double> vel(0.0, 7.0);
	static std::uniform_real_distribution<double> rvel(-0.2, 0.2);
	me = new Ship(&world);
	me->mirror = false;
	me->pos = { 0.0, 0.0 };
	me->vel = { 3.0, 5.0 };
	me->rpos = 0.0;
	me->rvel = 0.1;
	me->radius = 4.0;
	me->mass = 5000.0;
	me->rinertia = 5000.0;
	e1 = new Explosion(&world, *me, 200.0);
	for(int k = 0; k != 24; ++k)
	{
		Ship *obj = new Ship(&world);
		double a = phi(gen);
		double v = vel(gen);
		obj->mirror = false;
		obj->pos = { pos(gen), pos(gen) };
		obj->vel = { v * std::cos(a), v * std::sin(a) };
		obj->rpos = phi(gen);
		obj->rvel = rvel(gen);
		obj->radius = 4.0;
		obj->mass = 5000.0;
		obj->rinertia = 5000.0;
	}
}

double advanceFrameRateCounter(double dt)
{
	long static frames = 0;
	double static time = 0.0;
	double static rate = 0.0;
	++frames;
	time += dt;
	if(time >= 1.0)
	{
		rate = frames / time;
		frames = 0;
		time = 0;
	}
	return rate;
}

void step()
{
	static double const scale = 10.0;
	long const t_now = SDL_GetTicks();
	double const t = 0.001 * t_now;
	double const dt = 0.001 * (t_now - t_base);
	double const fps = advanceFrameRateCounter(dt);
	t_base = t_now;

	world.prepare(dt);
	world.collide();
	world.move();

	e1->move(dt);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glScalef(scale, scale, scale);
	glTranslated(0, -10.0, 0.0);

	for(Ship *object: world.ships)
		object->draw(me);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	e1->draw(me);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	glLineWidth(5.0);
	glColor4f(1.0, 1.0, 1.0, 0.3);
	glRotated(-180.0 / M_PI * me->rpos, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	for(long k = 0; k != 32; ++k)
	{
		double r = 1.3 * me->radius;
		double phi = M_PI / 16.0 * k;
		glVertex2d(r * std::cos(phi), r * std::sin(phi));
	}
	glEnd();

	glLoadIdentity();
	glColor4f(0.0, 1.0, 0.0, 0.7);
	vglTextOutF(-390.0, 280.0, 14.0, 1.0, "FPS: %.1f", fps);
	vglTextOutF(-390.0, 260.0, 14.0, 1.0, "Ships: %d", world.ships.size());
	vglTextOutF(-390.0, 240.0, 14.0, 1.0, "Particles: %d", e1->particles.size());

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
	glutInit(&argc, argv);
	initSDL();
	initGL();
	init();
	run();
	SDL_Quit();
	return 0;
}
