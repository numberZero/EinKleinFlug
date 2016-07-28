#include "types.hxx"
#include <cstddef>
#include <ctime>
#include <random>
#include <GL/gl.h>
#include <GL/glext.h>
#include "ship.hxx"
#include "world.hxx"

struct Vertex
{
	GLfloat position[2];
	GLshort texture[2];
	Color color;
};

struct ParticleTexture
{
private:
	GLuint id = 0;
	void generate();

public:
	operator GLuint();
} texture;

void ParticleTexture::generate()
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int const w = 16;
	int const r = w / 2;
	float const c = 2.0 / (w - 2);
	float *data = new float[w * w];
	float *pix = data;
	for(int j = -r; j != r; ++j)
		for(int i = -r; i != r; ++i)
		{
			float u = c * i;
			float v = c * j;
			float result = 0.0;
			float dist_squared = u * u + v * v;
			if(dist_squared < 1.0)
				result = 1.0 - std::sqrt(dist_squared);
			*(pix++) = result;
		}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, w, 0, GL_RED, GL_FLOAT, data);
	GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	delete[] data;
	glBindTexture(GL_TEXTURE_2D, 0);
}

ParticleTexture::operator GLuint()
{
	if(!id)
		generate();
	return id;
}


ParticleSystem::ParticleSystem(World *world, Float particle_size) :
	world(world),
	particle_size(particle_size)
{
	world->particles.insert(this);
#ifdef USE_SHADERS
	glGenBuffers(1, &vertex_buffer);
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(1);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
#endif
}

ParticleSystem::~ParticleSystem()
{
#ifdef USE_SHADERS
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
#endif
}

bool ParticleSystem::viable() const
{
	return !particles.empty();
}

void ParticleSystem::move(Float dt)
{
	for(auto iter = particles.begin(); iter != particles.end(); )
	{
		auto ppart = iter++;
		world->manifold.stepState(*ppart, dt);
		ppart->life -= dt;
		if(ppart->life <= 0)
			particles.erase(ppart);
	}
}

void ParticleSystem::draw(BodyState const *base)
{
	if(particles.empty())
		return;
	Float const r = particle_size;
#ifdef USE_SHADERS
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vertex) * 4 * particles.size(), nullptr, GL_STREAM_DRAW);
	Vertex *v = reinterpret_cast<Vertex *>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	if(!v)
		throw std::runtime_error("Can’t draw the particle system: can’t map the vertex buffer");
#else
	Vertex *vbuf = new Vertex[4 * particles.size()];
	Vertex *v = vbuf;
#endif
	for(Particle const &part: particles)
	{
		PointState pt = part;
		world->manifold.remap(*base, pt);
		Float const x = pt.pos[0];
		Float const y = pt.pos[1];
		Color c = getColor(part);
		v->color = c;
		v->position[0] = x - r;
		v->position[1] = y - r;
		v->texture[0] = 0;
		v->texture[1] = 0;
		++v;
		v->color = c;
		v->position[0] = x + r;
		v->position[1] = y - r;
		v->texture[0] = 1;
		v->texture[1] = 0;
		++v;
		v->color = c;
		v->position[0] = x + r;
		v->position[1] = y + r;
		v->texture[0] = 1;
		v->texture[1] = 1;
		++v;
		v->color = c;
		v->position[0] = x - r;
		v->position[1] = y + r;
		v->texture[0] = 0;
		v->texture[1] = 1;
		++v;
	}
#ifdef USE_SHADERS
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif

	glPushMatrix();
	glRotated(-180.0 / M_PI * base->rpos, 0.0, 0.0, 1.0);
	if(base->mirror)
		glScaled(-1.0, 1.0, 1.0);
	glTranslated(-base->pos[0], -base->pos[1], 0.0);

#ifdef USE_SHADERS
	glBindVertexArray(1);
	glDrawArrays(GL_QUADS, 0, particles.size());
	glBindVertexArray(0);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &vbuf->position);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &vbuf->color);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_SHORT, sizeof(Vertex), &vbuf->texture);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_QUADS, 0, 4 * particles.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif

	glPopMatrix();

#ifndef USE_SHADERS
	delete[] vbuf;
#endif
}

Color ParticleSystem::getColor(Particle const &p)
{
	Float y = std::atan(p.life) / M_PI;
	return Color(y, y, y, 1.0);
}

Explosion::Explosion(World *world, PointState const &base, Float power) :
	ParticleSystem(world, 2.0),
	base_vel(2.5 * std::pow(power, 0.25)),
	base_life(0.5 * std::log(power + 1.0) + 0.1)
{
	auto seed = std::time(nullptr);
	std::ranlux24 gen(seed);
	static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
	static std::uniform_real_distribution<Float> rad(0.0, 1.0);
	std::uniform_real_distribution<Float> vel(0.0, 2.0 * base_vel);
	static std::uniform_real_distribution<Float> dlife(0.9, 1.1);
	while(power > 0)
	{
		Particle p;
		Float a = phi(gen);
		Float r = rad(gen);

		Float b = phi(gen);
		Float v = vel(gen);

		p.pos = base.pos + r * Vector2{ std::cos(a), std::sin(a) };
		p.vel = base.vel + v * Vector2{ std::cos(b), std::sin(b) };
		p.value = base_vel / (v + 10.0);
		p.life = base_life * (0.5 + 0.025 * v) * dlife(gen);
		particles.push_back(p);
		power -= p.value;
	}
}

Color Explosion::getColor(Particle const &p)
{
	Float v = p.value;
	Float w = std::pow(p.life / base_life, 3.0);
	return Color(w, w * (3.0 * v - 1.0), w * (4.0 * v - 2.0));
}

Jet::Jet(Ship *ship, Vector2 shift, Vector2 thrust):
	ParticleSystem(ship->world, 0.5),
	full_thrust(thrust.norm()),
	base_vel(25.0),
	base_life(0.4),
	pos_spread(0.2),
	vel_spread(0.2 * base_vel),
	size_fullpower(250.0),
	particle_energy(base_life / size_fullpower),
	part_vel(-base_vel / full_thrust * thrust),
	ship(ship),
	pos(shift),
	thrust(thrust)
{
}

bool Jet::viable() const
{
	return !!ship; // permanent PS
}

void Jet::die()
{
	ship = nullptr;
}

void Jet::move(Float dt)
{
	ParticleSystem::move(dt);
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
	std::uniform_real_distribution<Float> dv(0.0, vel_spread);
	std::uniform_real_distribution<Float> dp(0.0, pos_spread);
	std::uniform_real_distribution<Float> ddt(-0.5 * dt, 0.5 * dt);
	static std::uniform_real_distribution<Float> dlife(0.9, 1.1);
	power = std::min(1.0, std::max(0.0, power));
	energy += power * dt;
	Float dvel = std::sqrt(power);
	while(energy > 0.0)
	{
		energy -= particle_energy;
		Particle p;
		p.pos = pos;
		p.vel = dvel * part_vel;
		Float a = phi(gen);
		Float r = dv(gen);
		p.vel[0] += r * std::cos(a);
		p.vel[1] += r * std::sin(a);
		a = phi(gen);
		r = dp(gen);
		p.pos[0] += r * std::cos(a);
		p.pos[1] += r * std::sin(a);
		p.pos += ddt(gen) * p.vel;
		p.value = particle_energy * p.vel.dot(part_vel) / (dvel * base_vel * base_vel);
		p.life = dlife(gen) * base_life * p.value / particle_energy;
		world->manifold.absolutize(*ship, p);
		particles.push_back(p);
	}
}

Color Jet::getColor(Particle const &p)
{
	Float w = std::pow(p.life / base_life, 3.0);
	Float v = p.value * w / particle_energy;
	return Color(2.5 * v, 2.5 * v - 1.0, 5.0 * v - 4.0);
}

Beam::Beam(Ship *ship, Vector2 shift, Vector2 vel, Float power, Float range):
	ParticleSystem(ship->world, 0.4),
	base_vel(vel.norm()),
	base_life(range / base_vel),
	pos_spread(0.3),
	vel_spread(0.04 * base_vel),
	size_fullpower(400.0 * power),
	particle_energy(base_life * power / size_fullpower),
	ship(ship),
	pos(shift),
	vel(vel),
	power(power)
{
}

// particels / second = power / particle_energy
// size_fullpower = base_life * particels / second = base_life * power / particle_energy
// particle_energy = base_life * power / size_fullpower

bool Beam::viable() const
{
	return !!ship; // permanent PS
}

void Beam::die()
{
	ship = nullptr;
}

void Beam::move(Float dt)
{
	ParticleSystem::move(dt);
	static std::ranlux24 gen(std::time(nullptr));
	static std::uniform_real_distribution<Float> phi(-M_PI, M_PI);
	std::uniform_real_distribution<Float> dv(0.0, vel_spread);
	std::uniform_real_distribution<Float> dp(0.0, pos_spread);
	std::uniform_real_distribution<Float> ddt(-0.5 * dt, 0.5 * dt);
	static std::uniform_real_distribution<Float> dlife(0.9, 1.1);
	if(shots)
		energy += power * dt;
	while(energy > 0.0)
	{
		energy -= particle_energy;
		Particle p;
		p.pos = pos;
		p.vel = vel;
		Float a = phi(gen);
		Float r = dv(gen);
		p.vel[0] += r * std::cos(a);
		p.vel[1] += r * std::sin(a);
		a = phi(gen);
		r = dp(gen);
		p.pos[0] += r * std::cos(a);
		p.pos[1] += r * std::sin(a);
		p.pos += ddt(gen) * p.vel;
		p.value = particle_energy * p.vel.dot(vel) / (base_vel * base_vel);
		p.life = dlife(gen) * base_life * p.value / particle_energy;
		world->manifold.absolutize(*ship, p);
		particles.push_back(p);
	}
}

Color Beam::getColor(Particle const &p)
{
	Float w = std::pow(p.life / base_life, 3.0);
	Float v = p.value * w / particle_energy;
	return Color(2.5 * v - 1.0, 5.0 * v - 4.0, 2.5 * v);
}
