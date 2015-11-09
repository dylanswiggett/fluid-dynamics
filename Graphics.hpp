#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_

#include <string>
#include "SDL2/SDL.h"
#include "GL/glew.h"
#include "GL/gl.h"
#include "Dynamics.hpp"
#include "DummyRender.hpp"

using namespace std;

class Graphics {
private:
  SDL_Window *window_;
  Dynamics *dynamics_;
  DummyRender *dummy_;
  GLuint shader_;
  
public:
  Graphics(SDL_Window *window, Dynamics *dynamics,
	   string vert_path, string frag_path,
	   DummyRender *dummy);
  ~Graphics();
  
  void update();
};

#endif // GRAPHICS_H_
