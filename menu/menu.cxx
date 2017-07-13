#include "menu.hxx"
#include "GL/gl.h"
#include <list>

extern Mainloop *mainloop;
extern SDL_Window *window;
Game game;
extern void init();

button btn1{-200,200,400,100,"PLAY",2};
button btn2{-200,50,400,100,"HELP",2};
button btn3{-200,-100,400,100,"QUIT",2};

bool button::isinside(double posx, double posy){
	int win_w,win_h,glx,gly;
	SDL_GL_GetDrawableSize(window,&win_w,&win_h);
	glx = int(win_w*posx/800 - 400);
	gly = int(300 - win_h*posy/600);
	
	if (glx>=x&&gly<=y&&glx<=x+width&&gly>=y-height)
		return true;
	else return false;
}

void button::ifinside(bool t){
	if(t) textwidth = 5;
	else textwidth = 2;
}
void Menu::step(){
Menu::draw();
}

void Menu::draw(){
SDL_Event event;

static bool t;

glClearColor(0.0, 0.0, 0.0, 1.0);
glClear(GL_COLOR_BUFFER_BIT);
glLoadIdentity();
int i = 0;
if(SDL_PollEvent(&event)){
	switch(event.type){
		case SDL_MOUSEMOTION:
				for(int i = 0; i<vector.size(); i++){
				if(i == 0){
				t = vector[0].isinside(event.motion.x,event.motion.y);}
				if (vector[i].isinside(event.motion.x, event.motion.y))
					vector[i].textwidth = 5;
				else vector[i].textwidth = 2;}
				
				
		case SDL_MOUSEBUTTONUP:
			if(t&&event.button.button == SDL_BUTTON_LEFT){
				init();
			mainloop = &game;
			}
			break;}
		
}
if(t){
	vglTextOutF(0,0,10,1,"mouseoversth");
}
for(int i = 0; i<vector.size(); i++){
	
	vector[i].drawbutton();
}

//glFlush();
//glFinish();
SDL_GL_SwapWindow(window);

//drawbutton(-200,-100,400,100,5,"QUIT");
}
