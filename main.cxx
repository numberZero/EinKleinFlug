#include <iostream>
#include <Eigen/Core>
#include <GL/gl.h>
#include <SDL.h>

SDL_Window *window;
static SDL_GLContext context;
static long t_base;

struct PointState
{
	bool flip;
	Eigen::Vector2d pos;
	Eigen::Vector2d vel;

	virtual void stepState(double dt);
};

struct BodyState: PointState
{
	double rpos;
	double rvel;

	void stepState(double dt) override;
};

void PointState::stepState(double dt)
{
	pos += dt * vel;
	if(pos[0] >= 50.0)
		pos[0] -= 100.0;
	if(pos[0] <= -50.0)
		pos[0] += 100.0;
	if(pos[1] >= 50.0)
	{
		pos[1] -= 100.0;
		flip = !flip;
	}
	if(pos[1] <= -50.0)
	{
		pos[1] += 100.0;
		flip = !flip;
	}
}

void BodyState::stepState(double dt)
{
	PointState::stepState(dt);
	rpos += dt * rvel;
}

BodyState example;
void init()
{
	example.flip = false;
	example.pos = { 0.0, 0.0 };
	example.vel = { 15.0, 20.0 };
	example.rpos = 0.0;
	example.rvel = 1.0;
}

void step()
{
	long const t_now = SDL_GetTicks();
	double const dt = 0.001 * (t_now - t_base);
	t_base = t_now;

	example.stepState(dt);

	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glScalef(5.0, 5.0, 5.0);

	glColor4f(1.0, 1.0, 1.0, 0.5);
	glBegin(GL_LINE_LOOP);
	glVertex2d(-50.0, -50.0);
	glVertex2d(50.0, -50.0);
	glVertex2d(50.0, 50.0);
	glVertex2d(-50.0, 50.0);
	glEnd();

	glPointSize(12.0);
	glPushMatrix();
	glTranslated(example.pos[0], example.pos[1], 0.0);
	if(example.flip)
		glScaled(-1.0, 1.0, 1.0);
	glRotatef(180.0 / M_PI * example.rpos, 0.0, 0.0, 1.0);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex2d(-10.0, 0.0);
	glVertex2d(10.0, 0.0);
	glVertex2d(3.0, 0.0);
	glVertex2d(0.0, 5.0);
	glVertex2d(3.0, 0.0);
	glVertex2d(0.0, -5.0);
	glVertex2d(1.0, 0.0);
	glVertex2d(-2.0, -5.0);
	glEnd();
	glPopMatrix();

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
