// C++ Program following the tutorial here:
// https://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// To load up the classic newell teapot (downloaded from Wikipedia)
#include <iostream>
#include <vector>
#include <cstring>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLES2/gl2.h>

extern "C" {
  #include "../shader_loader.h"
}
#include "object_loader.hpp"


const char* cubePath = "../data/teapot.obj";
const char* teapotPath = "../data/newell_teaset/teapot.obj";

const unsigned int sizeX = 1920;
const unsigned int sizeY = 1080;

const char* fragmentShaderPath = "shaders/shader.frag";
const char* vertexShaderPath = "shaders/shader.vert";


int main(int argc, char* argv[]) {

  // array variables to store the data
  // TB TODO: Do this in a class.
  std::vector< glm::vec3 > cube_1_vertices;
  std::vector< glm::vec2 > cube_1_uvs;
  std::vector< glm::vec3 > cube_1_normals;

  std::vector< glm::vec3 > cube_2_vertices;
  std::vector< glm::vec2 > cube_2_uvs;
  std::vector< glm::vec3 > cube_2_normals;
  
  if( loadTriangleOBJ(teapotPath, cube_1_vertices, cube_1_uvs, cube_1_normals) ) {
    std::cout << "cube 1 done" << std::endl;
  } else {
    std::cout << "no cube 1 :(" << std::endl;
  }

  if( loadTriangleOBJ(cubePath, cube_2_vertices, cube_2_uvs, cube_2_normals) ) {
    std::cout << "cube 2 done" << std::endl;
  } else {
    std::cout << "no cube 2 :(" << std::endl;
  }

  //printf("output vertices has %d elements\n", vertices.size());
  // for(uint i = 0; i < vertices.size(); i++) {
  //   printf("%f %f %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
  // }

  // We have now loaded up the teapot! Time to render it up
  // using the same techniques as the previous tutorial.

  SDL_Init(SDL_INIT_VIDEO);
  SDL_ShowCursor(SDL_DISABLE);

  SDL_Window *window;
  window = SDL_CreateWindow("Teapot window",
			    SDL_WINDOWPOS_UNDEFINED,
			    SDL_WINDOWPOS_UNDEFINED,
			    sizeX, sizeY,
			    SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

  // We are drawing to OpenGLESv2
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  
  if (window) {
    std::cout << "Created SDL OpenGL window" << std::endl;
  } else {
    std::cout << "Error: Could not create window: " << SDL_GetError() << std::endl;
  }

  // Open GL context
  SDL_GLContext glcontext = SDL_GL_CreateContext(window);

  // Create a VBO for the object vertex data.
  GLuint vertexVBO_cube_1;
  glGenBuffers(1, &vertexVBO_cube_1);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_cube_1);
  glBufferData(GL_ARRAY_BUFFER, cube_1_vertices.size() * sizeof(glm::vec3),
	       &cube_1_vertices[0], GL_STATIC_DRAW);

  GLuint vertexVBO_cube_2;
  glGenBuffers(1, &vertexVBO_cube_2);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_cube_2);
  glBufferData(GL_ARRAY_BUFFER, cube_2_vertices.size() * sizeof(glm::vec3),
	       &cube_2_vertices[0], GL_STATIC_DRAW);

  // MVP matrix for the scene.
  glm::mat4 Model = glm::mat4(1.0f);
  glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), // camera location
			       glm::vec3(0.0f, 0.0f, 0.0f), // looking at origin
			       glm::vec3(0.0f, 1.0f, 0.0f)  // setting up direction
			       );

  glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
					  (float) sizeX / (float) sizeY,
					  0.1f, 100.0f);

  // Set up the shader programs
  GLuint programID = load_shaders(vertexShaderPath, fragmentShaderPath);
  glUseProgram(programID);

  // Get uniform and attribute locations
  GLuint MVP_id = glGetUniformLocation(programID, "MVP");
  GLuint position_attr_i = glGetAttribLocation(programID, "vPosition");

  // Set some OpenGLES params
  SDL_GL_SetSwapInterval(1);
  glClearColor(0.0f, 0.4f, 0.2f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Here is our render loop!
  SDL_Event event;
  bool shouldExit = false;
  glm::mat4 mvp;
  while( !shouldExit ) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    while( SDL_PollEvent(&event) != 0) {
      if(event.type == SDL_KEYDOWN) {
	shouldExit = true;
	break;
      }
    }

    // Insert the MVP and do some rotations if necessary.
    Model = glm::rotate(Model, 0.01f, glm::vec3(0.0, 1.0, 0.1));
    mvp = Projection * View * Model;
    glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &mvp[0][0]);
    
    // Bind the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_cube_1);
    glVertexAttribPointer(position_attr_i,
			  3,
			  GL_FLOAT,
			  GL_FALSE,
			  0,
			  (void*) (0*sizeof(GLfloat)));
    glEnableVertexAttribArray(position_attr_i);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, cube_1_vertices.size());

    SDL_GL_SwapWindow(window);

    //glDisableVertexAttribArray(position_attr_i);

  }


  // Clean up
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return EXIT_SUCCESS;

}
