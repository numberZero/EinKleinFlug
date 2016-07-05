#include <iostream>
#include <Eigen/Core>
#include <GL/gl.h>
#include <SDL.h>

SDL_Window *window;
static SDL_GLContext context;
static long t_base;

struct PointState
{
	int map;
	bool flip;
	Eigen::Vector2d pos;
	Eigen::Vector2d vel;

	virtual void stepState(double dt);
	virtual void flipX();
	virtual void flipY();
};

struct BodyState: PointState
{
	double rpos;
	double rvel;

	void stepState(double dt) override;
	void flipX() override;
	void flipY() override;
};

struct MapTransferDescription
{
	int target;
	bool flip;
};

static bool const NONORIENTABLE = true;
static bool const PROJECTIVE_PLANE = false;

MapTransferDescription mtd_xp[] = {
	[0] = { .target = 2, .flip = false },
	[1] = { .target = 3, .flip = false },
	[2] = { .target = 0, .flip = PROJECTIVE_PLANE },
	[3] = { .target = 1, .flip = PROJECTIVE_PLANE },
};

MapTransferDescription mtd_xn[] = {
	[0] = { .target = 2, .flip = PROJECTIVE_PLANE },
	[1] = { .target = 3, .flip = PROJECTIVE_PLANE },
	[2] = { .target = 0, .flip = false },
	[3] = { .target = 1, .flip = false },
};

MapTransferDescription mtd_yp[] = {
	[0] = { .target = 1, .flip = NONORIENTABLE },
	[1] = { .target = 0, .flip = false },
	[2] = { .target = 3, .flip = NONORIENTABLE },
	[3] = { .target = 2, .flip = false },
};

MapTransferDescription mtd_yn[] = {
	[0] = { .target = 1, .flip = false },
	[1] = { .target = 0, .flip = NONORIENTABLE },
	[2] = { .target = 3, .flip = false },
	[3] = { .target = 2, .flip = NONORIENTABLE },
};

void PointState::stepState(double dt)
{
	pos += dt * vel;
	if(pos[0] >= 50.0)
	{
		pos[0] -= 50;
		if(mtd_xp[map].flip)
			flipY();
		map = mtd_xp[map].target;
	}
	if(pos[0] <= -50.0)
	{
		pos[0] += 50;
		if(mtd_xn[map].flip)
			flipY();
		map = mtd_xn[map].target;
	}
	if(pos[1] >= 50.0)
	{
		pos[1] -= 50;
		if(mtd_yp[map].flip)
			flipX();
		map = mtd_yp[map].target;
	}
	if(pos[1] <= -50.0)
	{
		pos[1] += 50;
		if(mtd_yn[map].flip)
			flipX();
		map = mtd_yn[map].target;
	}
}

void PointState::flipX()
{
	pos[0] = -pos[0];
	vel[0] = -vel[0];
	flip = !flip;
}

void PointState::flipY()
{
	pos[1] = -pos[1];
	vel[1] = -vel[1];
	flip = !flip;
}

void BodyState::stepState(double dt)
{
	PointState::stepState(dt);
	rpos += dt * rvel;
}

void BodyState::flipX()
{
	PointState::flipX();
	rpos = -rpos;
	rvel = -rvel;
}

void BodyState::flipY()
{
	PointState::flipY();
	rpos = -rpos;
	rvel = -rvel;
}

BodyState example;
void init()
{
	example.map = 0;
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
	Eigen::Vector2d pos = example.pos;
	switch(example.map)
	{
		case 0:
			break;
		case 1:
			pos[1] -= 50.0;
			break;
		case 2:
			pos[0] -= 50.0;
			break;
		case 3:
			pos[0] -= 50.0;
			pos[1] -= 50.0;
			break;
	}
	if(pos[0] <= -50.0)
	{
		if(PROJECTIVE_PLANE)
			glScaled(1.0, -1.0, 1.0);
		pos[0] += 100.0;
	}
	if(pos[1] <= -50.0)
	{
		if(NONORIENTABLE)
			glScaled(-1.0, 1.0, 1.0);
		pos[1] += 100.0;
	}
	glTranslated(pos[0], pos[1], 0.0);
	glRotatef(180.0 / M_PI * example.rpos, 0.0, 0.0, 1.0);
	if(example.flip)
		glScaled(-1.0, 1.0, 1.0);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glBegin(GL_LINES);
// 	glVertex2dv(example.pos.data());
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
