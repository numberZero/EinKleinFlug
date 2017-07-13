#include "game.hxx"
#include "physics/world.hxx"
extern void respawn();
extern void control();
SDL_Window *window;
World world(100.0);

int respawn_count = -1;
std::shared_ptr<Ship> me;
std::shared_ptr<Beam> b;

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
 void Game::draw()
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
	Float x = -390.0;
	Float y = 300.0;
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

void Game :: step()
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

	Game::draw();

	if(move)
	{
		control();
		world.move();
	}
  }


