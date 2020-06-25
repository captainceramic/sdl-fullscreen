#ifndef CUBE_HEADER
#define CUBE_HEADER

#include <gsl/gsl_matrix.h>
#include <SDL/SDL_opengl.h>

typedef struct {
  /* C struct to hold the information about our cubes:
     - The address of the relevant shader program
     - Pointer to an array of vertices
     - Pointer to an array of normals
     - Pointer to an array of uvs
     - A model matrix (though I haven't worked out how to do that in plain C
   */

  GLuint shaderProgramAddress;
  GLuint vertexVBO;
  uint num_triangles;
  gsl_matrix *model_matrix;
  GLfloat *vertices;
  GLfloat *normals;
  GLfloat *uvs;
  
} Cube;

#endif
