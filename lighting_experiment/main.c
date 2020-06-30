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
#include <cglm/cglm.h>

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
  glm_mat4_identity(thisCube.model_matrix);
  
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
}

void create_view_matrix(mat4* view_matrix_ptr) {
  vec3 eye = GLM_VEC3_ZERO_INIT;
  eye[2] = 8.0;
  eye[1] = 0.5;
  
  vec3 centre = GLM_VEC3_ZERO_INIT;

  vec3 up = GLM_VEC3_ZERO_INIT;
  up[1] = 1.0;
  
  glm_lookat(eye, centre, up, *view_matrix_ptr);
}

void create_projection_matrix(mat4* projection_matrix_ptr) {
  glm_mat4_identity(*projection_matrix_ptr);
  glm_perspective(glm_rad(45.0f),
		  (float) sizeX / (float) sizeY,
		  0.1f, 100.0f,
		  *projection_matrix_ptr);
}

int main(int argc, char* argv[]) {  

  set_up();

  Cube cube_1 = create_cube("../data/cube.obj");
  Cube cube_2 = create_cube("../data/cube.obj");

  /* Next up: we need view and projection matrices
     these stay constant */
  mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
  create_view_matrix(&view_matrix);

  mat4 projection_matrix = GLM_MAT4_IDENTITY_INIT;
  create_projection_matrix(&projection_matrix);

  /* Set up the openGLES shader program
     and assign it to the relevant cubes */
  GLuint basicShader = load_shaders(vertexShaderPath, fragmentShaderPath);
  cube_1.shaderProgramAddress = basicShader;
  cube_2.shaderProgramAddress = basicShader;

  /* Get the location of the vPosition attribute in the shader program */
  GLint position_attr_i = glGetAttribLocation(basicShader, "vPosition");

  /* Get the location for the uniforms */
  GLint model_ix = glGetUniformLocation(basicShader, "model");
  GLint view_ix = glGetUniformLocation(basicShader, "view");
  GLint perspective_ix = glGetUniformLocation(basicShader, "perspective");

  /* Set up the vertex buffer for cube 1 */
  glGenBuffers(1, &cube_1.vertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, cube_1.vertexVBO);
  // Send the vertices to OpenGL.
  glBufferData(GL_ARRAY_BUFFER,
	       cube_1.num_triangles * 3 * 3* sizeof(float),
	       cube_1.vertices,
	       GL_STATIC_DRAW);
  
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
   
    /* Render cube 1 */     
    glUseProgram(cube_1.shaderProgramAddress);

    glEnableVertexAttribArray(position_attr_i);
    glBindBuffer(GL_ARRAY_BUFFER, cube_1.vertexVBO);
    glVertexAttribPointer(position_attr_i, 3,
			  GL_FLOAT, GL_FALSE,
			  3*sizeof(GLfloat),
			  (void*)(0*sizeof(GLfloat)));

    /* Apply a rotation to the cube_1 model matrix */
    vec3 rotAxis = GLM_VEC3_ZERO_INIT;
    rotAxis[1] = 1.0f;
    rotAxis[0] = 0.2f;
    glm_rotate(cube_1.model_matrix,
	       0.05f,
	       rotAxis);

    /* Next: apply the model, view and projection matrices
       Plan: send the M, V and P matrices into the shaders */
    glUniformMatrix4fv(model_ix, 1, GL_FALSE, cube_1.model_matrix[0]);
    glUniformMatrix4fv(view_ix, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(perspective_ix, 1, GL_FALSE, &projection_matrix[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 3 * cube_1.num_triangles);
     
    glDisableVertexAttribArray(position_attr_i);

    /* Rendering cube 2 will happen here */
    SDL_GL_SwapWindow(window);
   
  }
 
  /* Clean up functions */
  destroy_cube(&cube_1);
  destroy_cube(&cube_2);
  clean_up();

  return 0;
}

