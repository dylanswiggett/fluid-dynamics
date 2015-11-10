#include <iostream>
#include <cstdlib>
#include <ctime>
#include "SDL2/SDL.h"
#include "GL/glew.h"
#include "Graphics.hpp"
#include "Dynamics.hpp"
#include "DummyRender.hpp"

#define SDL_FLAGS SDL_INIT_EVERYTHING
#define SDL_OGL_FLAGS SDL_WINDOW_OPENGL
#define SDL_WIDTH 1400
#define SDL_HEIGHT 800

using namespace std;

SDL_Window *init_SDL() {
  int res;
  SDL_Window *window;

  res = SDL_Init(SDL_FLAGS);
  if (res == -1)
    return NULL;

  window = SDL_CreateWindow(
    "GL Whatnot", 0, 0, SDL_WIDTH, SDL_HEIGHT,
    SDL_OGL_FLAGS);

  // SUCCESS
  return window;
}

void destroy_SDL() {
  SDL_Quit();
}

SDL_GLContext init_GL(SDL_Window *window) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  SDL_GLContext context = SDL_GL_CreateContext(window);

  glewExperimental = GL_TRUE;
  glewInit();

  return context;
}

void destroy_GL(SDL_GLContext context) {
  SDL_GL_DeleteContext(context);
}

int runGame() {
  bool running;
  char const* err_msg = NULL;
  SDL_Window *window;
  SDL_GLContext glcontext;
  SDL_Event event;
  Graphics *graphics;
  Dynamics *dynamics;
  DummyRender *dummy;

  // Setup

  if (!(window = init_SDL())) {
    cout << "Failed to initialize SDL. Exiting." << endl;
    return -1;
  }

  if (!(glcontext = init_GL(window))) {
    cout << "Failed to initialize openGL. Exiting." << endl;
    destroy_SDL();
    return -1;
  }

  cout << "Running with OpenGL version " << glGetString(GL_VERSION) << endl;

  running = true;

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    cout << "Initial setup error: " << err << endl;
  }

  try {

    // Prep for basic rendering.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // TODO: Add paths for shaders.
    dummy    = new DummyRender();
    dynamics = new Dynamics(SDL_WIDTH, SDL_HEIGHT, "shader.vert", "dynamics.frag", dummy);
    graphics = new Graphics(window, dynamics, "shader.vert", "graphics.frag", dummy);

    srand(time(NULL));

    GLubyte init_density[SDL_WIDTH * SDL_HEIGHT * 3] {0};
    GLubyte init_velocity[SDL_WIDTH * SDL_HEIGHT * 3] {0};
    for (int i = 0; i < SDL_WIDTH * SDL_HEIGHT; i++) {
      init_density[i * 3] = 100 * ((double)rand() / RAND_MAX);
      init_density[i * 3 + 1] = init_density[i*3 + 2] = 0x00;
      //init_density[i * 3 + 1] = 255 * ((double)rand() / RAND_MAX);
      //init_velocity[i * 3] = 255 * ((double)rand() / RAND_MAX);
      //init_velocity[i * 3 + 1] = 255 * ((double)rand() / RAND_MAX);
      init_velocity[i * 3] = 127;
      init_velocity[i * 3 + 1] = 127;
      init_velocity[i * 3 + 2] = 127;
    }
    dynamics->set_density(init_density);
    dynamics->set_velocity(init_velocity);

    while (running) {

      // Catch SDL events.
      while (SDL_PollEvent(&event)) {
	switch (event.type) {
        case SDL_QUIT:
          running = false;
	}
      }

      for (int i = 0; i < 6; i++)
	dynamics->update();
      graphics->update();

      SDL_GL_SwapWindow(window);      
      //SDL_Delay(500.0);
      SDL_Delay(10.0);

      GLenum err = glGetError();
      if (err != GL_NO_ERROR) {
	cout << "GLSL execution error: " << err << endl;
      }
    }
    
    delete graphics;
    delete dynamics;
    delete dummy;

  } catch (char const *msg) {
    err_msg = msg;
  }

  // Cleanup

  destroy_GL(glcontext);
  destroy_SDL();

  if (err_msg != NULL)
    throw err_msg;
  else
    return 0;
}

/* Top level just prints out any uncaught string error messages */
int main(int argc, char **argv) {
  try {
    return runGame();
  } catch (char const *msg) {
    cout << "Terminating after uncaught exception:\n> " << msg << endl;
    return -1;
  }
}
