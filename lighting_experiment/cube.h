#ifndef CUBE_HEADER
#define CUBE_HEADER

#include <cglm/cglm.h>
#include <SDL/SDL_opengl.h>

typedef struct {
  /* C struct to hold the information about our cubes:
     - The address of the relevant shader program
     - The ID of the relevant VBOs for data.
     - The number of triangles in the cube
     - Pointer to an array of vertices
     - Pointer to an array of normals
     - Pointer to an array of uvs
     - A model matrix
   */

  GLuint shaderProgramAddress;
  GLuint vertexVBO;
  uint num_triangles;
  GLfloat *vertices;
  GLfloat *normals;
  GLfloat *uvs;
  mat4 model_matrix;  
} Cube;

#endif
