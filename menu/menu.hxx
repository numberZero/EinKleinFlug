#pragma once
#include "GL/gl.h"
#include "GL/glut.h"
#include "visual/text.hxx"
#include "common/mainloop.hxx"
class menu : public Mainloop{
	void step() override;
	void draw() override;
};