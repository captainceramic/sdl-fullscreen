#include <iostream>
#include <SDL2/SDL.h>
#include <GLES2/gl2.h>

extern "C" {
  #include "shader_loader.h"
}
// This code is based on some example code at:
// https://wiki.libsdl.org/SDL_CreateWindow (and elsewhere on the SDL wiki)

// Set some parameters
const int sizeX = 1280;
const int sizeY = 720;

const char* fragmentShaderPath = "shaders/shader.frag";
const char* vertexShaderPath = "shaders/shader.vert";

int main(int argc, char* argv[]) {

  SDL_Init(SDL_INIT_VIDEO);

  // Not interested in the cursor.
  SDL_ShowCursor(SDL_DISABLE);
  
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

  // Set up the vertices and Vertex Buffer Object

  // Our triangle
  static const GLfloat g_vertex_buffer_data[] =
    {
     -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
    };

  GLuint VBO;
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
   	       g_vertex_buffer_data, GL_STATIC_DRAW);

  // Set up the shader program.
  GLuint programID = load_shaders(vertexShaderPath, fragmentShaderPath);
  
  if (glcontext) {
    std::cout << "\tOpenGLES version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "\tVendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "\tRenderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "\tShading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Set the interval for vsync
    SDL_GL_SetSwapInterval(1);

  } else {
    std::cout << "Error: Could not create OpenGLES context" << std::endl;
  }

  bool shouldExit = false;
  SDL_Event event;

  // Set the clear colour and clear
  glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
  
  while(!shouldExit) {

    while (SDL_PollEvent(&event) != 0) {
      if(event.type == SDL_KEYDOWN) {
	shouldExit = true;
	break;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw stuff here.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
    			  3 * sizeof(GLfloat), (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);

    glUseProgram(programID);
    
    SDL_GL_SwapWindow(window);
    
  }
  
  // Clean up
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return EXIT_SUCCESS;

} 
