#ifndef DUMMY_RENDER_HPP_
#define DUMMY_RENDER_HPP_

#include "GL/gl.h"

class DummyRender {
private:
  GLfloat vertices[12] = {1,1,0,  1,-1,0,  -1,-1,0,  -1,1,0}; // Square
  int indices[6] = {0,1,2,  0,2,3};
  GLuint vertex_buffer_;
  GLuint element_buffer_;
public:
  DummyRender();
  ~DummyRender();

  void draw();
};

#endif // DUMMY_RENDER_HPP_
