#include "menu.hxx"
#include "GL/gl.h"
extern SDL_Window *window;

button btn1{-200,200,400,100,"PLAY",2};
button btn2{-200,50,400,100,"HELP",2};
button btn3{-200,-100,400,100,"QUIT",2};

void Menu::step(){
Menu::draw();
}
void Menu::draw(){
	
glClearColor(0.0, 0.0, 0.0, 1.0);
glClear(GL_COLOR_BUFFER_BIT);
glLoadIdentity();
btn1.drawbutton();
btn2.drawbutton();
btn3.drawbutton();

//glFlush();
//glFinish();
SDL_GL_SwapWindow(window);

//drawbutton(-200,-100,400,100,5,"QUIT");
}
