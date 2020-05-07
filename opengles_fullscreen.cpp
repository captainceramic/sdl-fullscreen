#include <iostream>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>

// This code is based on some example code at:
// https://wiki.libsdl.org/SDL_CreateWindow

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
     
  if (window) {
    std::cout << "Created SDL OpenGL window" << std::endl;
  } else {
    std::cout << "Error: Could not create window: " << SDL_GetError() << std::endl;
  }
  
  SDL_Delay(6000);

  // Print out renderer information.
  std::cout << "Printing out render info:" << std::endl;
  std::cout << glGetString(GL_VERSION) << std::endl;
  std::cout << glGetString(GL_VENDOR) << std::endl;
  std::cout << glGetString(GL_RENDERER) << std::endl;
  std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  // Clean up
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return EXIT_SUCCESS;

} 
