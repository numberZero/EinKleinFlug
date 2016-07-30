#include <ctime>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <GL/gl.h>
#include <GL/glut.h>
#include <SDL.h>
#include "network/base.hxx"
#include "network/client.hxx"
#include "network/server.hxx"
#include "particles/beam.hxx"
#include "particles/jet.hxx"
#include "ship.hxx"
#include "text.hxx"
#include "world.hxx"

SDL_Window *window;
static SDL_GLContext context;
static long t_base;
std::uint8_t const *keys;

std::unique_ptr<Client> client;
std::unique_ptr<Server> server;
World world(100.0);
std::shared_ptr<Ship> me;
std::shared_ptr<Ship> he;
std::shared_ptr<Beam> b = nullptr;

static std::ranlux24 gen(std::time(nullptr));
static std::uniform_real_distribution<Float> pos(-world.manifold.radius, world.manifold.radius);
static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
static std::uniform_real_distribution<Float> vel(0.0, 7.0);
static std::uniform_real_distribution<Float> rvel(-0.2, 0.2);

void respawn(std::shared_ptr<Ship> &who = me, bool hero = true)
{
	Float a = phi(gen);
	Float v = vel(gen);
	who = Ship::create(&world);
	if(hero)
	{
		who->max_hp = 20.0;
		who->hp = 20.0;
		who->armor = 20.0;
	}
	who->prepare();
	who->mirror = false;
	who->pos = { pos(gen), pos(gen) };
	who->vel = { v * std::cos(a), v * std::sin(a) };
	who->rpos = phi(gen);
	who->rvel = 0.0;
	who->radius = 4.0;
	who->mass = 5000.0;
	who->rinertia = 5000.0;
	b = Beam::create(me, {0.0, 3.0}, {0.0, 150.0}, 20.0, 300.0);
}

void generateShips()
{
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

void init()
{
	if(!client)
	{
		generateShips();
		respawn(me);
	}
	if(server)
	{
		respawn(he);
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
	me->jets[0]->power = p_left > 0 ? p_left : 0.0;
	me->jets[1]->power = p_right > 0 ? p_right : 0.0;
	me->jets[2]->power = p_left < 0 ? -p_left : 0.0;
	me->jets[3]->power = p_right < 0 ? -p_right : 0.0;
	b->shots = shot;
}

void draw()
{
	static Float const scale = 10.0;

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glScalef(scale, scale, scale);
	glTranslated(0, -5.0, 0.0);
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
	Float x = -390.0;
	Float y = 300.0;
	Float dy = 20.0;
	Float w = 1.0;
	Float h = 14.0;
	glColor4f(0.0, 1.0, 0.0, 0.7);
	vglTextOutF(x, y -= dy, h, w, "FPS: %.1f / %.1f", fps_draw.value(), fps_model.value());
	vglTextOutF(x, y -= dy, h, w, "Ships: %d", world.ships.size());
	vglTextOutF(x, y -= dy, h, w, "Particle systems: %d", world.particles.size());
	glColor4f(1.0, 1.0, 0.0, 0.7);
#ifndef NDEBUG
	if(me)
		vglTextOutF(x, y -= dy, h, w, "MY Position: (%.1f, %.1f, %.2f, %s)", me->pos[0], me->pos[1], me->rpos, me->mirror ? "negative" : "positive");
	if(he)
		vglTextOutF(x, y -= dy, h, w, "HIS Position: (%.1f, %.1f, %.2f, %s)", he->pos[0], he->pos[1], he->rpos, he->mirror ? "negative" : "positive");
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

	if(client)
	{
		fps_model.advance(dt);
		client->recvState();
		world.cleanup();
		draw();
		return;
	}

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
			respawn(me);
		if(server && !he->viable())
			respawn(he);
		world.cleanup();
		if(server)
			server->sendState(he->id);
	}

	draw();

	if(move)
	{
		control();
		world.move();
	}
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
					respawn(me);
				if(event.key.keysym.scancode == SDL_SCANCODE_Y)
					respawn(he);
				break;
		}
	}
	return true;
}

void run()
{
	try
	{
		while(events())
			step();
	}
	catch(GracefulShutdown const &)
	{
		std::cout << "Disconnect" << std::endl;
	}
}

void initSDL()
{
	std::string title = "Ein Klein Flug";
	if(client)
		title += " (client)";
	if(server)
		title += " (server)";
	SDL_Init(SDL_INIT_EVERYTHING);
	keys = SDL_GetKeyboardState(nullptr);
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);
	SDL_GL_SetSwapInterval(0);
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
	switch(argc)
	{
		case 1:
			break;
		case 2:
			if(argv[1] == std::string("server"))
			{
				server.reset(new Server(&world));
				std::cout << "Waiting for connection" << std::endl;
				server->accept1();
				std::cout << "Connection accepted" << std::endl;
			}
			else if(argv[1] == std::string("client"))
			{
				client.reset(new Client(&world));
				std::cout << "Connecting..." << std::endl;
				client->connectLB();
				std::cout << "Connected" << std::endl;
			}
			break;
		default:
			std::cout << "Usage:" << std::endl;
			std::cout << "    einkleinflug [client|server]" << std::endl;
			return EXIT_FAILURE;
	}
	initSDL();
	initGL();
	init();
	run();
	SDL_Quit();
	return EXIT_SUCCESS;
}
