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
const char* redShaderPath = "shaders/red_shader.frag";
const char* blueShaderPath = "shaders/blue_shader.frag";

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

  printf("\nAt creation, cube vertices are:\n");
  for(int i=0; i<thisCube.num_triangles; i++) {
    printf("[ (%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f) ]\n",
	   thisCube.vertices[(i*9) + 0],
      	   thisCube.vertices[(i*9) + 1],
    	   thisCube.vertices[(i*9) + 2],
	   thisCube.vertices[(i*9) + 3],
      	   thisCube.vertices[(i*9) + 4],
    	   thisCube.vertices[(i*9) + 5],
   	   thisCube.vertices[(i*9) + 6],
      	   thisCube.vertices[(i*9) + 7],
    	   thisCube.vertices[(i*9) + 8]);
  }
  
  printf("\nAt creation, cube normals are:\n");
  for(int i=0; i<thisCube.num_triangles; i++) {
    printf("[ (%.2f %.2f %.2f) (%.2f %.2f %.2f) (%.2f %.2f %.2f) ]\n",
	   thisCube.normals[(i*9) + 0],
      	   thisCube.normals[(i*9) + 1],
    	   thisCube.normals[(i*9) + 2],
	   thisCube.normals[(i*9) + 3],
      	   thisCube.normals[(i*9) + 4],
    	   thisCube.normals[(i*9) + 5],
   	   thisCube.normals[(i*9) + 6],
      	   thisCube.normals[(i*9) + 7],
    	   thisCube.normals[(i*9) + 8]);
  }

  printf("\nAt creation, cube uvs are:\n");
  for(int i=0; i<thisCube.num_triangles; i++) {
    printf("[ (%.2f %.2f %.2f) (%.2f %.2f %.2f)\n",
	   thisCube.uvs[(i*6) + 0],
      	   thisCube.uvs[(i*6) + 1],
    	   thisCube.uvs[(i*6) + 2],
	   thisCube.uvs[(i*6) + 3],
      	   thisCube.uvs[(i*6) + 4],
    	   thisCube.uvs[(i*6) + 5]);
  }

  
  /* Set the default model matrix as the identity matrix */
  glm_mat4_identity(thisCube.model_matrix);
  
  /* Set up buffers for the vertices, normals and uvs */
  size_t vertexDataSize = thisCube.num_triangles * 3 * 3* sizeof(GLfloat);
  size_t uvDataSize = thisCube.num_triangles * 3 * 3* sizeof(GLfloat);

  glGenBuffers(1, &thisCube.vertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, thisCube.vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, vertexDataSize,
    	       thisCube.vertices, GL_STATIC_DRAW);
  
  glGenBuffers(1, &thisCube.normalVBO);
  glBindBuffer(GL_ARRAY_BUFFER, thisCube.normalVBO);
  glBufferData(GL_ARRAY_BUFFER, vertexDataSize,
    	       thisCube.normals, GL_STATIC_DRAW);
  
  glGenBuffers(1, &thisCube.uvVBO);
  glBindBuffer(GL_ARRAY_BUFFER, thisCube.uvVBO);
  glBufferData(GL_ARRAY_BUFFER, uvDataSize,
	       thisCube.uvs, GL_STATIC_DRAW);
  
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
  GLuint shader_red = load_shaders(vertexShaderPath, redShaderPath);
  cube_1.shaderProgramAddress = shader_red;

  GLuint shader_blue = load_shaders(vertexShaderPath, blueShaderPath);
  cube_2.shaderProgramAddress = shader_blue;

  /* Get the location of the vPosition attribute in the shader program */
  GLint position_attr_red = glGetAttribLocation(shader_red, "vPosition");
  GLint position_attr_blue = glGetAttribLocation(shader_blue, "vPosition");

  /* Get the location for the uniforms */
  GLint model_ix_red = glGetUniformLocation(shader_red, "model");
  GLint view_ix_red = glGetUniformLocation(shader_red, "view");
  GLint perspective_ix_red = glGetUniformLocation(shader_red, "perspective");

  GLint model_ix_blue = glGetUniformLocation(shader_blue, "model");
  GLint view_ix_blue = glGetUniformLocation(shader_blue, "view");
  GLint perspective_ix_blue = glGetUniformLocation(shader_blue, "perspective");
  
  /* Now - a main animation loop! */
  bool shouldExit = false;
  SDL_Event event;

  /* I want to move the second cube across */
  vec3 mv_vector = GLM_VEC3_ZERO_INIT;
  mv_vector[0] = 3.0f;
  glm_translate(cube_2.model_matrix, mv_vector);

  unsigned long int time_now = SDL_GetTicks();
  unsigned long int num_frames = 0;
  int time_gap;
  unsigned int new_time;
  int frame_max = 5 * 60;
  
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
    glEnableVertexAttribArray(position_attr_red);
    glBindBuffer(GL_ARRAY_BUFFER, cube_1.vertexVBO);
    glVertexAttribPointer(position_attr_red, 3,
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

    /* Next: apply the model, view and projection matrices */
    glUniformMatrix4fv(model_ix_red, 1, GL_FALSE, cube_1.model_matrix[0]);
    glUniformMatrix4fv(view_ix_red, 1, GL_FALSE, view_matrix[0]);
    glUniformMatrix4fv(perspective_ix_red, 1, GL_FALSE, projection_matrix[0]);

    glDrawArrays(GL_TRIANGLES, 0, 3 * cube_1.num_triangles);
     
    glDisableVertexAttribArray(position_attr_red);

    /* Render cube 2  */
    glUseProgram(cube_2.shaderProgramAddress);
    glEnableVertexAttribArray(position_attr_blue);
    glBindBuffer(GL_ARRAY_BUFFER, cube_2.vertexVBO);
    glVertexAttribPointer(position_attr_blue, 3,
			  GL_FLOAT, GL_FALSE,
			  3*sizeof(GLfloat),
			  (void*)(0*sizeof(GLfloat)));

    glUniformMatrix4fv(model_ix_blue, 1, GL_FALSE, cube_2.model_matrix[0]);
    glUniformMatrix4fv(view_ix_blue, 1, GL_FALSE, view_matrix[0]);
    glUniformMatrix4fv(perspective_ix_blue, 1, GL_FALSE, projection_matrix[0]);
    
    glDrawArrays(GL_TRIANGLES, 0, 3 * cube_2.num_triangles);
     
    glDisableVertexAttribArray(position_attr_blue);
    
    SDL_GL_SwapWindow(window);

    num_frames += 1;
    if(num_frames == frame_max) {
      new_time = SDL_GetTicks();
      time_gap = (new_time - time_now) / 1000;
      printf("current FPS: %.2f\n", (float) frame_max / (float) time_gap);
      time_now = new_time;
      num_frames = 0;
    };   
  }
 
  /* Clean up functions */
  destroy_cube(&cube_1);
  destroy_cube(&cube_2);
  clean_up();

  return 0;
}

