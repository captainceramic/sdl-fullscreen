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
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include "cube.h"
#include "object_loader.h"

/* Global parameters */
const int sizeX = 1920;
const int sizeY = 1080;

/* Set up global variables for the window and context. */
SDL_Window *window;
SDL_GLContext *glContext;


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
  if(loadOBJ(cube_filename, &thisCube)) {
    printf("loaded cube from file: %s\n", cube_filename);
  } else {
    printf("ERROR: file load %s failed\n", cube_filename);
  }

  /* Set the default model matrix as the identity matrix */
  thisCube.model_matrix = gsl_matrix_calloc(4, 4);
  gsl_matrix_set_identity(thisCube.model_matrix);
  
  return thisCube;
  
}

void destroy_cube(Cube * thisCube) {
  /* We need to free up the dynamically created arrays in the cubes */
  free(thisCube->vertices);
  free(thisCube->uvs);
  free(thisCube->normals);
  gsl_matrix_free(thisCube->model_matrix);
}

void create_view_matrix(gsl_matrix* view_matrix) {
  /* This is a function from
     https://www.3dgep.com/understanding-the-view-matrix/ 
  */

  gsl_matrix* orientation = gsl_matrix_calloc(4, 4);
  gsl_matrix_set_identity(orientation);

  gsl_matrix* translation = gsl_matrix_calloc(4, 4);
  gsl_matrix_set_identity(translation);

  /* Eye location is looking from the y axis, 5 units*/
  gsl_matrix_set(translation, 1, 3, -5.0);

  /* do the matrix multiplication */
  /* see: https://stackoverflow.com/questions/40687568/matrix-multiplication-using-gsl */
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
		 orientation, translation, 0.0,
		 view_matrix); 

  gsl_matrix_free(orientation);
  gsl_matrix_free(translation);
}

void create_projection_matrix(gsl_matrix* projection_matrix) {
}

int main(int argc, char* argv[]) {  

  set_up();

  Cube cube_1 = create_cube("../data/cube.obj");
  Cube cube_2 = create_cube("../data/cube.obj");

  /* /\* Have the cubes been created properly? *\/ */
  /* printf("number of triangles in cube 1: %d\n", cube_1.num_triangles); */
  /* printf("number of triangles in cube 2: %d\n", cube_2.num_triangles); */
  /* printf("first number is: %f\n", cube_1.vertices[0]); */
  /* printf("second number is: %f\n", cube_1.vertices[1]); */

  /* Next up: we need view and projection matrices */
  gsl_matrix* view_matrix = gsl_matrix_calloc(4, 4);
  create_view_matrix(view_matrix);

  /* How does our view matrix look? */
  /* for(int i=0; i<4; i++) { */
  /*   printf("[ %lf %lf %lf %lf ]", */
  /* 	   gsl_matrix_get(view_matrix, i, 0), */
  /* 	   gsl_matrix_get(view_matrix, i, 1), */
  /* 	   gsl_matrix_get(view_matrix, i, 2), */
  /* 	   gsl_matrix_get(view_matrix, i, 3)); */
  /* } */

  gsl_matrix* projection_matrix = gsl_matrix_calloc(4, 4);
  create_projection_matrix(projection_matrix);

  






  
  /* Clean up functions */
  destroy_cube(&cube_1);
  destroy_cube(&cube_2);
  gsl_matrix_free(view_matrix);
  gsl_matrix_free(projection_matrix); 
  clean_up();

  return 0;
}

