#pragma once
#include "GL/gl.h"
#include "GL/glut.h"
#include "visual/text.hxx"
#include "common/mainloop.hxx"
#include "SDL2/SDL.h"
extern bool mouseover;
class Menu : public Mainloop{
	void step() override;
	void draw() override;
};

class button{
public:
	double x; 
	double y; 
	double width; 
	double height; 
	 
	const char* text;
	double textwidth;
	double bottom;
	double right;
	
	void drawbutton(){
	glLoadIdentity();
	bottom = y - height;
	right = x + width;
	vglTextOutF(x+100,bottom+25, height-10, textwidth,text);
	glColor4f(1.0,0.5,0.0,1.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(x, y);
	glVertex2d(x, bottom);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2d(x,bottom);
	glVertex2d(right,bottom);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2d(right,bottom);
	glVertex2d(right,y);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2d(right,y);
	glVertex2d(x, y);
	glEnd();
	
	
}
};

