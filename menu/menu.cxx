#include "menu.hxx"
void menu::showmenu(float x, float y, float h, float w){
  glColor4f(1.0, 0.0, 0.0, 0.7);
  vglTextOutF(x, y -= 20.0, h, w, "menu");
}