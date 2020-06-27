/* A program to experiment with some of the lighting tutorials from
   https://learnopengl.com/ and https://www.opengl-tutorial.org/
   but ported to the pi (pre-pi4)

   The plan is to set up some orbiting cubes and do some reflections and things.

   The program will be written using SDL and OpenGLESv2

*/

#include <stdio.h>
#include <stdbool.h>

#include <GLES2/gl2.h>
#include <SDL2/SDL.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include "cube.h"
#include "object_loader.h"
#include "shader_loader.h"

/* Global parameters */
const int sizeX = 1920;
const int sizeY = 1080;
const char* vertexShaderPath = "shaders/shader.vert";
const char* fragmentShaderPath = "shaders/shader.frag";

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

    // Set the interval for vsync
    SDL_GL_SetSwapInterval(1);
    
    // Set the clear colour and enable depth testing
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
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
  thisCube.model_matrix = gsl_matrix_float_calloc(4, 4);
  gsl_matrix_float_set_identity(thisCube.model_matrix);

  /* Set up buffers for the vertices
     TB TODO: Buffers also for uvs and normals
 */
  glGenBuffers(1, &thisCube.vertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, thisCube.vertexVBO);

  size_t dataSize = thisCube.num_triangles * 3 * sizeof(GLfloat);
  glBufferData(GL_ARRAY_BUFFER, dataSize,
    	       thisCube.vertices, GL_STATIC_DRAW);
  
  printf("size of cube buffer data: %d\n", dataSize);
	     
  return thisCube;
  
}

void destroy_cube(Cube * thisCube) {
  /* We need to free up the dynamically created arrays in the cubes */
  free(thisCube->vertices);
  free(thisCube->uvs);
  free(thisCube->normals);
  gsl_matrix_float_free(thisCube->model_matrix);
}

void create_view_matrix(gsl_matrix_float* view_matrix) {
  /* This is a function from
     https://www.3dgep.com/understanding-the-view-matrix/ 
  */

  gsl_matrix_float* orientation = gsl_matrix_float_calloc(4, 4);
  gsl_matrix_float_set_identity(orientation);

  gsl_matrix_float* translation = gsl_matrix_float_calloc(4, 4);
  gsl_matrix_float_set_identity(translation);

  /* Eye location is looking from the y axis, 5 units*/
  gsl_matrix_float_set(translation, 1, 3, -5.0);

  /* do the matrix multiplication */
  /* see: https://stackoverflow.com/questions/40687568/matrix-multiplication-using-gsl */
  gsl_blas_sgemm(CblasNoTrans, CblasNoTrans, 1.0,
		 orientation, translation, 0.0,
		 view_matrix); 

  gsl_matrix_float_free(orientation);
  gsl_matrix_float_free(translation);
}

void create_projection_matrix(gsl_matrix_float* projection_matrix) {
  gsl_matrix_float_set_identity(projection_matrix);
}

int main(int argc, char* argv[]) {  

  set_up();

  Cube cube_1 = create_cube("../data/cube.obj");
  Cube cube_2 = create_cube("../data/cube.obj");

  /* Next up: we need view and projection matrices */
  gsl_matrix_float* view_matrix = gsl_matrix_float_calloc(4, 4);
  create_view_matrix(view_matrix);

  gsl_matrix_float* projection_matrix = gsl_matrix_float_calloc(4, 4);
  create_projection_matrix(projection_matrix);

  /* Set up the openGLES shader program
     and assign it to the relevant cubes */
  GLuint basicShader = load_shaders(vertexShaderPath, fragmentShaderPath);
  cube_1.shaderProgramAddress = basicShader;
  cube_2.shaderProgramAddress = basicShader;

  /* Get the location of the vPosition attribute in the shader program
     currently just for shader 1.
   */
  GLint position_attr_i = glGetAttribLocation(cube_1.shaderProgramAddress, "vPosition");
  GLint model_mat_i = glGetAttribLocation(cube_1.shaderProgramAddress, "mat_model");
  GLint view_mat_i = glGetAttribLocation(cube_1.shaderProgramAddress, "mat_view");
  GLint projection_mat_i = glGetAttribLocation(cube_1.shaderProgramAddress, "mat_projection");
  
  /* Now - a main animation loop! */
  bool shouldExit = false;
  SDL_Event event;
  while(!shouldExit) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    while (SDL_PollEvent(&event) != 0) {
      if(event.type == SDL_KEYDOWN) {
	shouldExit = true;
	break;
      }
    }

    /* Next: apply the model, view and projection matrices
       Plan: send the M, V and P matrices into the shaders */
    glUniformMatrix4fv(model_mat_i, 1, GL_FALSE, &cube_1.model_matrix->data[0][0]);
    glUniformMatrix4fv(view_mat_i, 1, GL_FALSE, &view_matrix->data[0][0]);
    glUniformMatrix4fv(projection_mat_i, 1, GL_FALSE, &projection_matrix->data[0][0]);
    
    /* Render cube 1 */
    glBindBuffer(GL_ARRAY_BUFFER, cube_1.vertexVBO);
    glVertexAttribPointer(position_attr_i, 3,
    			  GL_FLOAT, GL_FALSE,
    			  3*sizeof(GLfloat),
    			  (void*) (0*sizeof(GLfloat)));
    glEnableVertexAttribArray(position_attr_i);
    glUseProgram(cube_1.shaderProgramAddress);
    glDrawArrays(GL_TRIANGLES, 0, cube_1.num_triangles);
	
    SDL_GL_SwapWindow(window);
    
    glDisableVertexAttribArray(position_attr_i);

  }

   
  /* Clean up functions */
  destroy_cube(&cube_1);
  destroy_cube(&cube_2);
  gsl_matrix_float_free(view_matrix);
  gsl_matrix_float_free(projection_matrix); 
  clean_up();

  return 0;
}

