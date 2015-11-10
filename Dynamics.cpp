#include "Dynamics.hpp"
#include "shader_loader.hpp"
#include <iostream>
#include <vector>

using namespace std;

void buildTex(GLuint *loc, int w, int h)
{
  glGenTextures(1, loc);
  glBindTexture(GL_TEXTURE_2D, *loc);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "Dynamics buildTex Error: " << err << endl;
    exit(0);
  }

  vector<GLubyte> emptyData(w * h * 3, 0x00);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &emptyData[0]);
}

Dynamics::Dynamics(int w, int h,
		   string vert_path, string frag_path,
		   DummyRender *dummy) :
  w_(w), h_(h), dummy_(dummy)
{
  shader_ = LoadShaders(vert_path.c_str(), frag_path.c_str());

  glGenFramebuffers(1, &fb_);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "Dynamics creation Error: " << err << endl;
    exit(0);
  }

  buildTex(&density_tex_, w, h);
  buildTex(&velocity_tex_, w, h);
  buildTex(&density_tex_out_, w, h);
  buildTex(&velocity_tex_out_, w, h);

  setup_fb();
  GLuint clearColor[4] = {0, 0, 0, 0};
  glClearBufferuiv(GL_COLOR, 0, clearColor);
  glClearBufferuiv(GL_COLOR, 1, clearColor);
}

Dynamics::~Dynamics()
{
  glDeleteProgram(shader_);
  glDeleteFramebuffers(1, &fb_);

  glDeleteTextures(1, &density_tex_);
  glDeleteTextures(1, &velocity_tex_);
  glDeleteTextures(1, &density_tex_out_);
  glDeleteTextures(1, &velocity_tex_out_);
}

void Dynamics::swap_buffers()
{
  // TODO: Check if this actually works...
  GLuint density_temp = density_tex_;
  GLuint velocity_temp = velocity_tex_;
  density_tex_ = density_tex_out_;
  velocity_tex_ = velocity_tex_out_;
  density_tex_out_ = density_temp;
  velocity_tex_out_ = velocity_temp;
}

void Dynamics::setup_fb()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, density_tex_out_, 0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
			 GL_TEXTURE_2D, velocity_tex_out_, 0);
  
  GLenum drawbuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, drawbuffers);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "Dynamics setup_fb Error: " << err << endl;
    exit(0);
  }

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Failed to create Dynamics frame buffer." << std::endl;
    exit(1);
  }
}

void Dynamics::update()
{
  glUseProgram(shader_);
  setup_fb();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, density_tex_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, velocity_tex_);

  GLfloat loc;
  loc = glGetUniformLocation(shader_, DENSITY_VAR);
  glUniform1i(loc, 0);
  loc = glGetUniformLocation(shader_, VELOCITY_VAR);
  glUniform1i(loc, 1);

  loc = glGetUniformLocation(shader_, "w");
  glUniform1i(loc, w_);
  loc = glGetUniformLocation(shader_, "h");
  glUniform1i(loc, h_);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "Dynamics update Error: " << err << endl;
    exit(0);
  }
  
  dummy_->draw();

  glUseProgram(0);
  swap_buffers();
}

void Dynamics::set_density(GLubyte *arr)
{
  glBindTexture(GL_TEXTURE_2D, density_tex_);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w_, h_, GL_RGB, GL_UNSIGNED_BYTE, &arr[0]);
}

void Dynamics::set_velocity(GLubyte *arr)
{
  glBindTexture(GL_TEXTURE_2D, velocity_tex_);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w_, h_, GL_RGB, GL_UNSIGNED_BYTE, &arr[0]);
}
