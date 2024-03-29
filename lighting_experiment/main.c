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
const char* lightingShaderPath = "shaders/lighting_shader.frag";

/* Set up global variables for the window and context. */
SDL_Window *window;
SDL_GLContext *glContext;

void set_up() {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("SDL OpenGLES window",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    sizeX, sizeY,
			    SDL_WINDOW_OPENGL);

  if (window) {
    printf("Created SDL OpenGL window\n");
  } else {
    printf("Error: Could not create window\n");
  }

  // Not interested in the cursor.
  SDL_ShowCursor(SDL_DISABLE);

  // Create the actual context for GLESv2
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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

void create_view_matrix(mat4* view_matrix_ptr, vec3* view_vector_ptr) {

  vec3 centre = GLM_VEC3_ZERO_INIT;
  vec3 up = GLM_VEC3_ZERO_INIT;
  up[1] = 1.0;

  glm_lookat(*view_vector_ptr, centre, up, *view_matrix_ptr);
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
  Cube cube_3 = create_cube("../data/cube.obj");

  /* TODO: I want cube_2 to be small */
  /* glm_scale_uni(cube_2.model_matrix, 0.2f); */

  /* Next up: we need view and projection matrices
     these stay constant */
  mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
  vec3 view_position = GLM_VEC3_ZERO_INIT;
  view_position[2] = 16.0;
  view_position[1] = 0.5;
  create_view_matrix(&view_matrix, &view_position);

  printf("Our view position is: %f %f %f\n",
	 view_position[0], view_position[1], view_position[2]);
  
  mat4 projection_matrix = GLM_MAT4_IDENTITY_INIT;
  create_projection_matrix(&projection_matrix);

  /* Set up the openGLES shader program
     and assign it to the relevant cubes */
  GLuint shader_1 = load_shaders(vertexShaderPath, lightingShaderPath);
  GLuint shader_2 = load_shaders(vertexShaderPath, lightingShaderPath);
  GLuint shader_3 = load_shaders(vertexShaderPath, lightingShaderPath);

  cube_1.shaderProgramAddress = shader_1;
  cube_2.shaderProgramAddress = shader_2;
  cube_3.shaderProgramAddress = shader_3;

  /* Get the location of the vPosition attribute in the shader program */
  GLint position_attr_1 = glGetAttribLocation(shader_1, "vPosition");
  GLint position_attr_2 = glGetAttribLocation(shader_2, "vPosition");
  GLint position_attr_3 = glGetAttribLocation(shader_3, "vPosition");

  GLint normal_attr_1 = glGetAttribLocation(shader_1, "vNormal");
  GLint normal_attr_2 = glGetAttribLocation(shader_2, "vNormal");
  GLint normal_attr_3 = glGetAttribLocation(shader_2, "vNormal");  

  /* Get the location for the uniforms */
  GLint model_ix_1 = glGetUniformLocation(shader_1, "model");
  GLint view_ix_1 = glGetUniformLocation(shader_1, "view");
  GLint perspective_ix_1 = glGetUniformLocation(shader_1, "perspective");
  GLint mat_normal_ix_1 = glGetUniformLocation(shader_1, "mat_normal");
  
  GLint object_col_1 = glGetUniformLocation(shader_1, "objectColour");
  GLint lighting_col_1 = glGetUniformLocation(shader_1, "lightColour");
  GLint ambient_strength_1 = glGetUniformLocation(shader_1, "ambientStrength");
  GLint specular_strength_1 = glGetUniformLocation(shader_1, "specularStrength");
  GLint lighting_pos_1 = glGetUniformLocation(shader_1, "lightPos");
  GLint view_pos_1 = glGetUniformLocation(shader_1, "viewPos");
  
  GLint model_ix_2 = glGetUniformLocation(shader_2, "model");
  GLint view_ix_2 = glGetUniformLocation(shader_2, "view");
  GLint perspective_ix_2 = glGetUniformLocation(shader_2, "perspective");
  GLint mat_normal_ix_2 = glGetUniformLocation(shader_2, "mat_normal");
  
  GLint object_col_2 = glGetUniformLocation(shader_2, "objectColour");
  GLint lighting_col_2 = glGetUniformLocation(shader_2, "lightColour");
  GLint ambient_strength_2 = glGetUniformLocation(shader_2, "ambientStrength");
  GLint specular_strength_2 = glGetUniformLocation(shader_2, "specularStrength");
  GLint lighting_pos_2 = glGetUniformLocation(shader_2, "lightPos");
  GLint view_pos_2 = glGetUniformLocation(shader_2, "viewPos");

  GLint model_ix_3 = glGetUniformLocation(shader_3, "model");
  GLint view_ix_3 = glGetUniformLocation(shader_3, "view");
  GLint perspective_ix_3 = glGetUniformLocation(shader_3, "perspective");
  GLint mat_normal_ix_3 = glGetUniformLocation(shader_3, "mat_normal");
  
  GLint object_col_3 = glGetUniformLocation(shader_3, "objectColour");
  GLint lighting_col_3 = glGetUniformLocation(shader_3, "lightColour");
  GLint ambient_strength_3 = glGetUniformLocation(shader_3, "ambientStrength");
  GLint specular_strength_3 = glGetUniformLocation(shader_3, "specularStrength");
  GLint lighting_pos_3 = glGetUniformLocation(shader_3, "lightPos");
  GLint view_pos_3 = glGetUniformLocation(shader_3, "viewPos");
  
  /* Set the object and lighting colours for both cubes */
  vec3 white = GLM_VEC3_ONE_INIT;
  vec3 coral = GLM_VEC3_ZERO_INIT;
  coral[0] = 1.0f;
  coral[1] = 0.5f;
  coral[2] = 0.31f;
  vec3 green = GLM_VEC3_ZERO_INIT;
  green[1] = 1.0f;
  green[2] = 0.1f;
  
  /* Now - a main animation loop! */
  bool shouldExit = false;
  SDL_Event event;

  /* I want to move the second cube across and make it orbit
     the centre */
  vec3 cube_2_position_vector = GLM_VEC3_ZERO_INIT;
  cube_2_position_vector[0] = 3.0f;

  vec3 cube_3_position_vector = GLM_VEC3_ZERO_INIT;
  cube_3_position_vector[0] = -5.5f;

  vec3 z_axis = GLM_VEC3_ZERO_INIT;
  z_axis[2] = 1.0f;

  vec3 cube_2_translation_vector;
  glm_translate(cube_2.model_matrix, cube_2_position_vector);

  vec3 cube_3_translation_vector;
  glm_translate(cube_3.model_matrix, cube_3_position_vector);

  unsigned long int time_now = SDL_GetTicks();
  unsigned long int num_frames = 0;
  int time_gap;
  unsigned int new_time;
  int frame_max = 5 * 60;
  
  mat4 normal_matrix_1;
  mat4 normal_matrix_2;
  mat4 normal_matrix_3;
  
  while(!shouldExit) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    while (SDL_PollEvent(&event) != 0) {
      if(event.type == SDL_KEYDOWN) {
	shouldExit = true;
	break;
      }
    }

    /* Update any required positions */
    /* Apply a rotation to the cube_1 model matrix */
    vec3 rotAxis = GLM_VEC3_ZERO_INIT;
    rotAxis[1] = 1.0f;
    rotAxis[0] = 0.2f;
    glm_rotate(cube_1.model_matrix,
	       0.025f,
	       rotAxis);

    /* Rotate cube 3 */
    glm_rotate(cube_3.model_matrix,
	       0.05f,
	       rotAxis);
    
    /* I want the blue cube to orbit the red cube */
    glm_vec3_cross(cube_2_position_vector, z_axis, 
		   cube_2_translation_vector);

    glm_vec3_scale(cube_2_translation_vector, 0.01, cube_2_translation_vector);

    glm_vec3_add(cube_2_position_vector,
		 cube_2_translation_vector,
		 cube_2_position_vector);
    
    glm_translate(cube_2.model_matrix, cube_2_translation_vector);

    /* And the third cube */
    glm_vec3_cross(cube_3_position_vector, z_axis, 
		   cube_3_translation_vector);

    glm_vec3_scale(cube_3_translation_vector, 0.01, cube_3_translation_vector);

    glm_vec3_add(cube_3_position_vector,
		 cube_3_translation_vector,
		 cube_3_position_vector);
    
    glm_translate(cube_3.model_matrix, cube_3_translation_vector);
    
    /* Calculate the normal matrices */
    glm_mat4_inv(cube_1.model_matrix, normal_matrix_1);
    glm_mat4_transpose(normal_matrix_1);

    glm_mat4_inv(cube_2.model_matrix, normal_matrix_2);
    glm_mat4_transpose(normal_matrix_2);
    
    glm_mat4_inv(cube_3.model_matrix, normal_matrix_3);
    glm_mat4_transpose(normal_matrix_3);
    
    /* Render cube 1 */     
    glUseProgram(cube_1.shaderProgramAddress);

    /* Apply the model, view and projection matrices */
    glUniformMatrix4fv(model_ix_1, 1, GL_FALSE, cube_1.model_matrix[0]);
    glUniformMatrix4fv(view_ix_1, 1, GL_FALSE, view_matrix[0]);
    glUniformMatrix4fv(perspective_ix_1, 1, GL_FALSE, projection_matrix[0]);
    glUniformMatrix4fv(mat_normal_ix_1, 1, GL_FALSE, normal_matrix_1[0]);
    
    glUniform3fv(object_col_1, 1, white);
    glUniform3fv(lighting_col_1, 1, coral);
    glUniform3fv(lighting_pos_1, 1, cube_2_position_vector);
    glUniform3fv(view_pos_1, 1, view_position);
    glUniform1f(ambient_strength_1, 0.1f);
    glUniform1f(specular_strength_1, 0.5f);

    glEnableVertexAttribArray(normal_attr_1);
    glEnableVertexAttribArray(position_attr_1);
    
    /* Vertices */
    glBindBuffer(GL_ARRAY_BUFFER, cube_1.vertexVBO);
    glVertexAttribPointer(position_attr_1, 3,
			  GL_FLOAT, GL_FALSE,
			  3*sizeof(GLfloat),
			  (void*)(0*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, cube_1.normalVBO);
    glVertexAttribPointer(normal_attr_1, 3,
			  GL_FLOAT, GL_FALSE,
			  3*sizeof(GLfloat),
			  (void*)(0*sizeof(GLfloat)));
    
    glDrawArrays(GL_TRIANGLES, 0, 3 * cube_1.num_triangles);

    glDisableVertexAttribArray(position_attr_1);
    glDisableVertexAttribArray(normal_attr_1);
    
    /* Render cube 2  */
    glUseProgram(cube_2.shaderProgramAddress);

    glUniformMatrix4fv(model_ix_2, 1, GL_FALSE, cube_2.model_matrix[0]);
    glUniformMatrix4fv(view_ix_2, 1, GL_FALSE, view_matrix[0]);
    glUniformMatrix4fv(perspective_ix_2, 1, GL_FALSE, projection_matrix[0]);  
    glUniformMatrix4fv(mat_normal_ix_2, 1, GL_FALSE, normal_matrix_2[0]);
    
    glUniform3fv(object_col_2, 1, white);
    glUniform3fv(lighting_col_2, 1, white);
    glUniform3fv(lighting_pos_2, 1, cube_2_position_vector);
    glUniform3fv(view_pos_2, 1, view_position);
    glUniform1f(ambient_strength_2, 1.0f);
    glUniform1f(specular_strength_2, 0.0f);

    glEnableVertexAttribArray(position_attr_2);
    glEnableVertexAttribArray(normal_attr_2);

    glBindBuffer(GL_ARRAY_BUFFER, cube_2.vertexVBO);
    glVertexAttribPointer(position_attr_2, 3,
			  GL_FLOAT, GL_FALSE,
			  3*sizeof(GLfloat),
			  (void*)(0*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, cube_2.normalVBO);
    glVertexAttribPointer(normal_attr_2, 3,
			  GL_FLOAT, GL_FALSE,
			  3*sizeof(GLfloat),
			  (void*)(0*sizeof(GLfloat)));
    
    glDrawArrays(GL_TRIANGLES, 0, 3 * cube_2.num_triangles);

    glDisableVertexAttribArray(position_attr_2);
    glDisableVertexAttribArray(normal_attr_2);

    /* Render cube 3  */
    glUseProgram(cube_3.shaderProgramAddress);

    glUniformMatrix4fv(model_ix_3, 1, GL_FALSE, cube_3.model_matrix[0]);
    glUniformMatrix4fv(view_ix_3, 1, GL_FALSE, view_matrix[0]);
    glUniformMatrix4fv(perspective_ix_3, 1, GL_FALSE, projection_matrix[0]);  
    glUniformMatrix4fv(mat_normal_ix_3, 1, GL_FALSE, normal_matrix_3[0]);
    
    glUniform3fv(object_col_3, 1, green);
    glUniform3fv(lighting_col_3, 1, white);
    glUniform3fv(lighting_pos_3, 1, cube_2_position_vector);
    glUniform3fv(view_pos_3, 1, view_position);
    glUniform1f(ambient_strength_3, 0.1f);
    glUniform1f(specular_strength_3, 0.5f);

    glEnableVertexAttribArray(position_attr_3);
    glEnableVertexAttribArray(normal_attr_3);

    glBindBuffer(GL_ARRAY_BUFFER, cube_3.vertexVBO);
    glVertexAttribPointer(position_attr_3, 3,
			  GL_FLOAT, GL_FALSE,
			  3*sizeof(GLfloat),
			  (void*)(0*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, cube_3.normalVBO);
    glVertexAttribPointer(normal_attr_3, 3,
			  GL_FLOAT, GL_FALSE,
			  3*sizeof(GLfloat),
			  (void*)(0*sizeof(GLfloat)));
    
    glDrawArrays(GL_TRIANGLES, 0, 3 * cube_3.num_triangles);

    glDisableVertexAttribArray(position_attr_3);
    glDisableVertexAttribArray(normal_attr_3);
    
    SDL_GL_SwapWindow(window);

    /* Frame counter */
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
  destroy_cube(&cube_3);
  clean_up();

  return 0;
}

