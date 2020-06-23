/* A program to experiment with some of the lighting tutorials from
   https://learnopengl.com/ and https://www.opengl-tutorial.org/
   but ported to the pi (pre-pi4)

   The plan is to set up some orbiting cubes and do some reflections and things.

   The program will be written using SDL and OpenGLESv2

*/

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "object_loader.h"

/* Global parameters */
const int sizeX = 1920;
const int sizeY = 1080;

/* Set up global variables for the window and context. */
SDL_Window *window;
SDL_GLContext *glContext;

/* A C struct to hold the information about our cubes

   Contains:
   * An array of vertices
   * An array of normals
   * A pointer to a shader program

*/
typedef struct {
  /* The cube contains:
     - The address of the relevant shader program
     - Pointer to an array of vertices
     - Pointer to an array of normals
     - Pointer to an array of uvs
     - A model matrix (though I haven't worked out how to do that in plain C
   */

  uint shaderProgramAddress;
  float **vertices;
  float **normals;
  float **uvs;
  
} Cube;

void set_up() {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("SDL OpenGLES window",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    sizeX, sizeY,
			    SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

  if (window) {
    printf("Created SDL OpenGL window\n");
  } else {
    printf("Error: Could not create window\n");
  }

  // Not interested in the cursor.
  SDL_ShowCursor(SDL_DISABLE);

  // Create the actual context for GLESv2
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  glContext = SDL_GL_CreateContext(window);

  if (glContext) {
    printf("OpenGLES render information:\n");
    printf("\tOpenGLES version: %s\n", glGetString(GL_VERSION));
    printf("\tVendor: %s\n", glGetString(GL_VENDOR));
    printf("\tRenderer: %s\n", glGetString(GL_RENDERER));
    printf("\tShading Language Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  } else {
    printf("Error: Could not create OpenGL context\n");
  }
}

void clean_up() {
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

Cube create_cube(char* cube_filename) {
  /* This is a function that given a path to an OBJ object file will
     create a Cube struct with the relevant information.
  */

  Cube thisCube;
  if(loadOBJ(cube_filename, thisCube.vertices, thisCube.uvs, thisCube.normals)) {
    printf("loaded cube from file: %s\n", cube_filename);
  } else {
    printf("ERROR: file load %s failed\n", cube_filename);
  }

  return thisCube;
  
}

int main(int argc, char* argv[]) {  

  set_up();

  /* TB NOTE: Do I need to free the memory for these? */
  Cube cube_1 = create_cube("../data/cube.obj");

  /* Have the cubes been created properly? */
  printf("first number is: %f\n", cube_1.vertices[0]);

  clean_up();

  return 0;
}

