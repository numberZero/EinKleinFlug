#include <ctime>
#include <iostream>
#include <list>
#include <random>
#include <Eigen/Core>
#include <GL/gl.h>
#include <SDL.h>
#include "manifold.hxx"

SDL_Window *window;
static SDL_GLContext context;
static long t_base;

SquareKleinBottle m2{50.0};

std::list<BodyState> bodies;

void init()
{
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<double> pos{-50.0, 50.0};
	BodyState body;
	body.mirror = false;
	body.pos = { 0.0, 0.0 };
	body.vel = { 3.0, 5.0 };
	body.rpos = 0.0;
	body.rvel = 0.1;
	bodies.push_back(body);
	for(int k = 0; k != 12; ++k)
	{
		body.mirror = false;
		body.pos = { pos(gen), pos(gen) };
		body.vel = { 0.0, 0.0 };
		body.rpos = 0.0;
		body.rvel = 0.0;
		bodies.push_back(body);
	}
}

void step()
{
	static double const scale = 150.0 / m2.radius;
	long const t_now = SDL_GetTicks();
	double const dt = 0.001 * (t_now - t_base);
	t_base = t_now;

	for(BodyState &state: bodies)
		m2.stepState(state, dt);

	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glRotatef(90.0, 0.0, 0.0, 1.0);

	BodyState me = bodies.front();
	for(BodyState &state0: bodies)
	{
		glPushMatrix();
		BodyState state = state0;
		m2.relativize(me, state);
		glTranslated(state.pos[0], state.pos[1], 0.0);
		glColor4f(0.0, 1.0, 1.0, 0.5);
		glBegin(GL_LINES);
		glVertex2d(0.0, 0.0);
		glVertex2dv(state.vel.data());
		glEnd();
		if(state.mirror)
			glScaled(-1.0, 1.0, 1.0);
		glRotatef(180.0 / M_PI * state.rpos, 0.0, 0.0, 1.0);

		glColor4f(1.0, 1.0, 1.0, 0.3);
		glBegin(GL_LINES);
		glVertex2d(-4.0, 0.0);
		glVertex2d(4.0, 0.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, 2.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, -2.0);
		glVertex2d(0.0, 0.0);
		glVertex2d(-1.0, -2.0);
		glEnd();

		glPopMatrix();

	}

	glRotatef(-180.0 / M_PI * me.rpos, 0.0, 0.0, 1.0);
	glColor4f(1.0, 1.0, 1.0, 0.3);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-m2.radius, -m2.radius);
	glVertex2d(m2.radius, -m2.radius);
	glVertex2d(m2.radius, m2.radius);
	glVertex2d(-m2.radius, m2.radius);
	glEnd();
	if(me.mirror)
		glScaled(-1.0, 1.0, 1.0);
	glTranslated(-me.pos[0], -me.pos[1], 0.0);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	for(BodyState &state0: bodies)
	{
		glPushMatrix();
		BodyState state = state0;
		m2.remap(me, state);
		glTranslated(state.pos[0], state.pos[1], 0.0);
		if(state.mirror)
			glScaled(-1.0, 1.0, 1.0);
		glRotatef(180.0 / M_PI * state.rpos, 0.0, 0.0, 1.0);

		glBegin(GL_LINES);
		glVertex2d(-4.0, 0.0);
		glVertex2d(4.0, 0.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, 2.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, -2.0);
		glVertex2d(0.0, 0.0);
		glVertex2d(-1.0, -2.0);
		glEnd();
		glPopMatrix();

		glColor4f(1.0, 1.0, 0.0, 1.0);
	}

	glLoadIdentity();
	glTranslated(-200.0, 0.0, 0.0);
	glScalef(scale, scale, scale);

	glColor4f(1.0, 1.0, 1.0, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-m2.radius, -m2.radius);
	glVertex2d(m2.radius, -m2.radius);
	glVertex2d(m2.radius, m2.radius);
	glVertex2d(-m2.radius, m2.radius);
	glEnd();

	glColor4f(0.0, 1.0, 0.0, 1.0);
	for(BodyState &state: bodies)
	{
		glPushMatrix();
		glTranslated(state.pos[0], state.pos[1], 0.0);
		if(state.mirror)
			glScaled(-1.0, 1.0, 1.0);
		glRotatef(180.0 / M_PI * state.rpos, 0.0, 0.0, 1.0);

		glBegin(GL_LINES);
		glVertex2d(-4.0, 0.0);
		glVertex2d(4.0, 0.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, 2.0);
		glVertex2d(1.0, 0.0);
		glVertex2d(0.0, -2.0);
		glVertex2d(0.0, 0.0);
		glVertex2d(-1.0, -2.0);
		glEnd();
		glPopMatrix();

		glColor4f(1.0, 1.0, 0.0, 1.0);
	}

	glLoadIdentity();
	glTranslated(200.0, 0.0, 0.0);
	glScalef(scale, scale, scale);

	glColor4f(1.0, 1.0, 1.0, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-m2.radius, -m2.radius);
	glVertex2d(m2.radius, -m2.radius);
	glVertex2d(m2.radius, m2.radius);
	glVertex2d(-m2.radius, m2.radius);
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
