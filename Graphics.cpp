#include "Graphics.hpp"
#include "shader_loader.hpp"
#include <iostream>

using namespace std;

Graphics::Graphics(SDL_Window *window, Dynamics *dynamics,
		   string vert_path, string frag_path,
		   DummyRender *dummy) :
  window_(window), dynamics_(dynamics), dummy_(dummy)
{
  shader_ = LoadShaders(vert_path.c_str(), frag_path.c_str());
}

Graphics::~Graphics()
{
  glDeleteProgram(shader_);
}


void Graphics::update() {
  glUseProgram(shader_);
  
  // Draw to screen
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  // Enable textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, dynamics_->get_density_tex());
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, dynamics_->get_velocity_tex());

  GLfloat loc;
  loc = glGetUniformLocation(shader_, DENSITY_VAR);
  glUniform1i(loc, 0);
  loc = glGetUniformLocation(shader_, VELOCITY_VAR);
  glUniform1i(loc, 1);

  int w, h;
  SDL_GetWindowSize(window_, &w, &h);
  loc = glGetUniformLocation(shader_, "w");
  glUniform1i(loc, w);
  loc = glGetUniformLocation(shader_, "h");
  glUniform1i(loc, h);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "Graphics update Error: " << err << endl;
    exit(0);
  }
  
  dummy_->draw();

  glUseProgram(0);
}
