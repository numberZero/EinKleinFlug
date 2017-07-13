#pragma once
#include "GL/gl.h"
#include "GL/glut.h"
#include "visual/text.hxx"
#include "common/mainloop.hxx"
#include <SDL.h>
#include "list"
#include "logic/ship.hxx"
#include "physics/world.hxx"
#include "common/fpscounter.hxx"
#include "particles/beam.hxx"
static long t_base;
static bool st_slow = false;
static bool st_stabilizing = false;
class Game : public Mainloop{
public:
	void draw() override;
	void step() override;
};