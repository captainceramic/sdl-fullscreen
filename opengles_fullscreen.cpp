#include <iostream>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>

// This code is based on some example code at:
// https://wiki.libsdl.org/SDL_CreateWindow (and elsewhere on the SDL wiki)

// Set some parameters
const int sizeX = 1280;
const int sizeY = 720;

int main(int argc, char* argv[]) {

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window;
  window = SDL_CreateWindow("SDL OpenGLES window",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    sizeX, sizeY,
			    SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

  // Set the window up for OpenGLESv2
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  if (window) {
    std::cout << "Created SDL OpenGL window" << std::endl;
  } else {
    std::cout << "Error: Could not create window: " << SDL_GetError() << std::endl;
  }

  // Now create the actual context.
  SDL_GLContext glcontext = SDL_GL_CreateContext(window);

  if (glcontext) {
    std::cout << "Printing out render info:" << std::endl;
    std::cout << "\tOpenGLES version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "\tVendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "\tRenderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "\tShading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  } else {
    std::cout << "Error: Could not create OpenGLES context" << std::endl;
  }

  // Draw loop goes here!
  glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);
  SDL_Delay(6000);

  // Clean up
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return EXIT_SUCCESS;

} 
