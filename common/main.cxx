#include <ctime>
#include <iostream>
#include <list>
#include <random>
#include <GL/gl.h>
#include <GL/glut.h>
#include <SDL.h>
#include "particles/beam.hxx"
#include "particles/jet.hxx"
#include "logic/ship.hxx"
#include "visual/text.hxx"
#include "physics/world.hxx"

SDL_Window *window;
static SDL_GLContext context;
static long t_base;
std::uint8_t const *keys;

World world(100.0);
std::shared_ptr<Ship> me;
std::shared_ptr<Beam> b = nullptr;
int respawn_count = -1;

static std::ranlux24 gen(std::time(nullptr));
static std::uniform_real_distribution<Float> pos(-world.manifold.radius, world.manifold.radius);
static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
static std::uniform_real_distribution<Float> vel(0.0, 7.0);
static std::uniform_real_distribution<Float> rvel(-0.2, 0.2);

void respawn()
{
	Float a = phi(gen);
	Float v = vel(gen);
	++respawn_count;
	me = Ship::create(&world);
#ifdef HERO
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
	for(int k = 0; k != 24; ++k)
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

class FPSCounter
{
	long frames = 0;
	Float time = 0.0;
	Float rate = 0.0;

public:
	void advance(Float dt);
	Float value() const;
};

void FPSCounter::advance(Float dt)
{
	++frames;
	time += dt;
	if(time >= 1.0)
	{
		rate = frames / time;
		frames = 0;
		time = 0;
	}
}

Float FPSCounter::value() const
{
	return rate;
}

static FPSCounter fps_draw;
static FPSCounter fps_model;
static bool st_slow = false;
static bool st_stabilizing = false;

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

static Float w = 400.0, h = 300.0;

void draw()
{
	static Float const scale = 10.0;

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glScalef(scale, scale, scale);
	glTranslated(0, -10.0, 0.0);
	world.draw(me);
// mark “me”
	glLineWidth(5.0);
	glColor4f(0.0, 1.0, 0.0, 0.3);
	glRotated(-180.0 / M_PI * me->rpos, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	for(long k = 0; k != 32; ++k)
	{
		Float r = 1.3 * me->radius;
		Float phi = M_PI / 16.0 * k;
		glVertex2d(r * std::cos(phi), r * std::sin(phi));
	}
	glEnd();

	glLoadIdentity();
	Float x = -w + 10.0;
	Float y = h;
	Float dy = 20.0;
	Float w = 1.0;
	Float h = 14.0;
	glColor4f(0.0, 1.0, 0.0, 0.7);
	vglTextOutF(x, y -= dy, h, w, "FPS: %.1f / %.1f", fps_draw.value(), fps_model.value());
	vglTextOutF(x, y -= dy, h, w, "Respawns: %d", respawn_count);
	vglTextOutF(x, y -= dy, h, w, "Ships: %d", world.ships.size());
	vglTextOutF(x, y -= dy, h, w, "Particle systems: %d", world.particles.size());
	vglTextOutF(x, y -= dy, h, w, "Entities: %d", world.entities.size());
	glColor4f(1.0, 1.0, 0.0, 0.7);
#ifndef NDEBUG
	vglTextOutF(x, y -= dy, h, w, "Position: (%.1f, %.1f, %s)", me->pos[0], me->pos[1], me->mirror ? "positive" : "negative");
#endif
	if(st_stabilizing)
		vglTextOutF(x, y -= dy, h, w, "Stabilizing");
	if(st_slow)
	{
		glColor4f(1.0, 0.0, 0.0, 0.7);
		vglTextOutF(x, y -= dy, h, w, "Lag");
	}

	glFlush();
	glFinish();
	SDL_GL_SwapWindow(window);
}

void step()
{
	long const t_now = SDL_GetTicks();
	Float const dt = 0.001 * (t_now - t_base);
	static Float t_add = 0.0;
	t_add += dt;
	bool const move = t_add > 0;
	st_slow = t_add > world.dt;
	t_base = t_now;

	fps_draw.advance(dt);

	if(move)
	{
		double t_add_old = t_add;
		t_add -= world.dt;
		if(t_add > 0)
			t_add = 0;
		fps_model.advance(t_add_old - t_add);
		world.prepare();
		world.collide();
		if(!me->viable())
			respawn();
		world.cleanup();
	}

	draw();

	if(move)
	{
		control();
		world.move();
	}
}

void resize(int width, int height)
{
	glViewport(0, 0, width, height);

	float scale = 500.0;
	float d = sqrt(width * width + height * height);
	float u = width / d;
	float v = height / d;
	w = scale * u;
	h = scale * v;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w, w, -h, h, -10, 100);
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
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						resize(event.window.data1, event.window.data2);
						break;
				}
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
	keys = SDL_GetKeyboardState(nullptr);
	window = SDL_CreateWindow("Ein Klein Flug", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

	glEnable(GL_FRAMEBUFFER_SRGB);

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
