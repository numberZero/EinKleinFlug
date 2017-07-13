#pragma once
#include "GL/gl.h"
#include "GL/glut.h"
#include "visual/text.hxx"
#include "common/mainloop.hxx"
#include <SDL.h>
#include "vector"
#include "game/game.hxx"


class button{
public:
	double x; 
	double y; 
	double width; 
	double height; 
	 
	const char* text;
	double textwidth;
	bool isinside(double posx, double posy);
	void ifinside(bool t);
	
	void drawbutton(){
	glLoadIdentity();
	double bottom = y - height;
	double right = x + width;
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
class Menu : public Mainloop{
public:
	std::vector <button> vector;
	Menu(std::vector<button> vector){this->vector=vector;};
	void step() override;
	void draw() override;
};
