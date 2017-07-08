#include "menu.hxx"
#include "SDL2/SDL.h"
#include "GL/gl.h"
extern SDL_Window *window;
class button{
public:
double x; 
double y; 
double width; 
double height; 
double textwidth; 
const char* text;
void drawbutton(){
	double bottom = y - height;
	double right = x + width;
	glColor4f(1.0,0.0,0.0,1.0);
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
	vglTextOutF(x+100,bottom+25, height-10, textwidth,text);
	}
};
button btn1{-200,200,400,100,5,"PLAY"};
button btn2{-200,50,400,100,5,"HELP"};
button btn3{-200,-100,400,100,5,"QUIT"};
void menu::step(){
	

	menu::draw();


}
void menu::draw(){
	
glClearColor(0.0, 0.0, 0.0, 1.0);
glClear(GL_COLOR_BUFFER_BIT);
glLoadIdentity();
btn1.drawbutton();
btn2.drawbutton();
btn3.drawbutton();
glFlush();
glFinish();
SDL_GL_SwapWindow(window);
//drawbutton(-200,-100,400,100,5,"QUIT");
}
