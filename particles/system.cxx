#include "system.hxx"
#include <GL/gl.h>
#include <GL/glext.h>
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

void ParticleSystem::move()
{
	for(auto iter = particles.begin(); iter != particles.end(); )
	{
		auto ppart = iter++;
		world->manifold.stepState(*ppart, world->dt);
		ppart->life -= world->dt;
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
	if(colorization)
	{
		Float a = p.value;
		Float b = p.life * p.life * p.life;
		return Color(Vector4(*colorization * Vector4{ a, b, a * b, 1.0 }));
	}
	Float y = std::atan(p.life) / M_PI;
	return Color(y, y, y, 1.0);
}
