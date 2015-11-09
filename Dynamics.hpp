#ifndef DYNAMICS_HPP_
#define DYNAMICS_HPP_

#include <string>
#include "SDL2/SDL.h"
#include "GL/glew.h"
#include "GL/gl.h"
#include "DummyRender.hpp"

using namespace std;

#define DENSITY_VAR "density"
#define VELOCITY_VAR "velocity"

class Dynamics {
private:
  GLuint density_tex_, velocity_tex_;
  GLuint density_tex_out_, velocity_tex_out_;
  GLuint fb_;
  GLuint shader_;
  int w_, h_;
  DummyRender *dummy_;

  void swap_buffers();
  void setup_fb();

public:
  Dynamics(int w, int h,
	   string vert_path, string frag_path,
	   DummyRender *dummy);
  ~Dynamics();

  void update();
  GLuint get_density_tex() { return density_tex_; }
  GLuint get_velocity_tex() { return velocity_tex_; }

  int get_w() { return w_; }
  int get_h() { return h_; }

  void set_density(GLubyte *arr);
  void set_velocity(GLubyte *arr);
};

#endif // DYNAMICS_HPP_
