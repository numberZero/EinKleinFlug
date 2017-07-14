#include <ctime>
#include <iostream>
#include "vector"
#include <random>
#include <GL/gl.h>
#include <GL/glut.h>
#include <SDL.h>
#include "particles/beam.hxx"
#include "particles/jet.hxx"
#include "logic/ship.hxx"
#include "visual/text.hxx"
#include "physics/world.hxx"
#include "menu/menu.hxx"
#include "mainloop.hxx"
#include "game/game.hxx"
#include "fpscounter.hxx"
static SDL_GLContext context;
std::uint8_t const *keys;
World world(100.0);
extern button btn1;
extern button btn2;
extern button btn3;
extern SDL_Window *window;
extern Game game;
extern int respawn_count;
extern std::shared_ptr<Ship> me;
extern std::shared_ptr<Beam> b;
extern long t_base;
extern bool st_stabilizing;
extern bool st_slow;
std::vector <button> vector {btn1,btn2,btn3};
Menu menu(vector);
Mainloop *mainloop=&menu;

static std::ranlux24 gen(std::time(nullptr));
static std::uniform_real_distribution<Float> pos(-world.manifold.radius, world.manifold.radius);
static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
static std::uniform_real_distribution<Float> vel(0.0, 7.0);
static std::uniform_real_distribution<Float> rvel(-0.2, 0.2);

void respawn()
{
	Float a = phi(gen);
	Float v = vel(gen);
	respawn_count++;
	me = Ship::create(&world);
#ifndef HERO
	me->max_hp = 20.0;
	me->hp = 20.0;
	me->armor = 20.0;
#endif
	me->prepare();
	me->mirror = false;
	me->pos = { pos(gen), pos(gen) };
	me->vel = { v * std::cos(a), v * std::sin(a) };
	me->rpos = phi(gen);
	me->rvel = 0.0;
	me->radius = 4.0;
	me->mass = 5000.0;
	me->rinertia = 5000.0;
	b.reset(new Beam(*me, {0.0, 3.0}, {0.0, 150.0}, 20.0, 300.0));
}

void init()
{
	respawn();
	
	for(unsigned int k = 0; k != 24; ++k)
	{
		std::shared_ptr<Ship> obj(Ship::create(&world));
		Float a = phi(gen);
		Float v = vel(gen);
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






void control()
{
	bool stabilize = keys[SDL_SCANCODE_S];
	bool shot = keys[SDL_SCANCODE_SPACE];
	bool up = keys[SDL_SCANCODE_UP];
	bool down = keys[SDL_SCANCODE_DOWN];
	bool left = keys[SDL_SCANCODE_LEFT];
	bool right = keys[SDL_SCANCODE_RIGHT];
	Float const p_base = 1.0;
	Float const p_rot = 0.1;
	Float const p_stab = p_rot;
	Float p_left 	= (up ? p_base : 0.0) + (down ? -p_base : 0.0) + (left ? -p_rot : 0.0) + (right ? p_rot : 0.0);
	Float p_right 	= (up ? p_base : 0.0) + (down ? -p_base : 0.0) + (left ? p_rot : 0.0) + (right ? -p_rot : 0.0);
	st_stabilizing = stabilize;
	if(stabilize)
	{
		static Float const eps = 0.01;
		left = false;
		right = false;
		if(me->rvel > eps)
		{
			p_left += p_stab;
			p_right -= p_stab;
		}
		else if(me->rvel < -eps)
		{
			p_left -= p_stab;
			p_right += p_stab;
		}
		else
			st_stabilizing = false;
	}
	me->jets[0]->setPower(p_left);
	me->jets[1]->setPower(p_right);
	me->jets[2]->setPower(-p_left);
	me->jets[3]->setPower(-p_right);
	b->setShot(shot);
}


void resize()
{
	int window_width, window_height;
	SDL_GL_GetDrawableSize(window, &window_width, &window_height);
	glViewport(0, 0, window_width, window_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double viewport_half_width = std::max(400.0, 300.0 * window_width / window_height);
	double viewport_half_height = std::max(300.0, 400.0 * window_height / window_width);
	glOrtho(-viewport_half_width, viewport_half_width, -viewport_half_height, viewport_half_height, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
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
			case SDL_KEYDOWN:
				if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					return false;
				if(event.key.keysym.scancode == SDL_SCANCODE_X)
					respawn();
				if(event.key.keysym.scancode == SDL_SCANCODE_M){
					if(mainloop == &game)
					mainloop = &menu;			
				}
				break;	
		}
	}
	return true;
}

void run()
{
	while(events()) {
		resize();
		mainloop->step();
	}
}

void initSDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	keys = SDL_GetKeyboardState(nullptr);
	window = SDL_CreateWindow(
		"Ein Klein Flug",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800,
		600,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		);
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);
	SDL_GL_SetSwapInterval(-1);
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
