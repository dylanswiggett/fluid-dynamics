#include "GL/glew.h"
#include "GL/gl.h"
#include "DummyRender.hpp"
#include <iostream>

using namespace std;

DummyRender::DummyRender()
{
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "DummyRender VBO creation Error: " << err << endl;
    exit(0);
  }

  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "DummyRender EAB creation Error: " << err << endl;
    exit(0);
  }
}

DummyRender::~DummyRender()
{
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &element_buffer_);
}

void DummyRender::draw()
{
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "DummyRender Error when draw called: " << err << endl;
    exit(0);
  }
   
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

  err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "DummyRender vertex array Error: " << err << endl;
    exit(0);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

  glDisableVertexAttribArray(0);

  err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "DummyRender draw Error: " << err << endl;
    exit(0);
  }
}
