#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GLES2/gl2.h>
#include <glm/gtc/matrix_transform.hpp>

extern "C" {
  #include "shader_loader.h"
}

// This code is based on some example code at:
// https://wiki.libsdl.org/SDL_CreateWindow (and elsewhere on the SDL wiki)
// https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/

// Set some parameters
const int sizeX = 1920;
const int sizeY = 1080;

const char* fragmentShaderPath = "shaders/shader.frag";
const char* vertexShaderPath = "shaders/shader.vert";
const char* texturePath = "image/texture.png";

int main(int argc, char* argv[]) {

  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);

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

  // Now - I'm going to merge the geometry and the colours into a singleVBO.
  static const GLfloat g_vertex_buffer_data[] =
  {
   // Vertex ... (x3)    Colour ... (x3)            Texture coords (x2)         
    -1.0f,-1.0f,-1.0f,  0.583f,  0.771f,  0.014f, 
    -1.0f,-1.0f, 1.0f,  0.609f,  0.115f,  0.436f,
    -1.0f, 1.0f, 1.0f,  0.327f,  0.483f,  0.844f,
    1.0f, 1.0f,-1.0f,   0.822f,  0.569f,  0.201f,
    -1.0f,-1.0f,-1.0f,  0.435f,  0.602f,  0.223f,
    -1.0f, 1.0f,-1.0f,  0.310f,  0.747f,  0.185f,
    1.0f,-1.0f, 1.0f,   0.597f,  0.770f,  0.761f,
    -1.0f,-1.0f,-1.0f,  0.559f,  0.436f,  0.730f,  
    1.0f,-1.0f,-1.0f,   0.359f,  0.583f,  0.152f,
    1.0f, 1.0f,-1.0f,   0.483f,  0.596f,  0.789f,
    1.0f,-1.0f,-1.0f,   0.167f,  0.620f,  0.077f,   
    -1.0f,-1.0f,-1.0f,  0.279f,  0.317f,  0.505f,
    -1.0f,-1.0f,-1.0f,  0.945f,  0.719f,  0.592f,
    -1.0f, 1.0f, 1.0f,  0.014f,  0.184f,  0.576f,
    -1.0f, 1.0f,-1.0f,  0.140f,  0.616f,  0.489f,
    1.0f,-1.0f, 1.0f,   0.771f,  0.328f,  0.970f,
    -1.0f,-1.0f, 1.0f,  0.195f,  0.548f,  0.859f,
    -1.0f,-1.0f,-1.0f,  0.676f,  0.977f,  0.133f,
    -1.0f, 1.0f, 1.0f,  0.559f,  0.861f,  0.639f,
    -1.0f,-1.0f, 1.0f,  0.971f,  0.572f,  0.833f,
    1.0f,-1.0f, 1.0f,   0.997f,  0.513f,  0.064f,
    1.0f, 1.0f, 1.0f,   0.406f,  0.615f,  0.116f,
    1.0f,-1.0f,-1.0f,   0.543f,  0.021f,  0.978f,
    1.0f, 1.0f,-1.0f,   0.347f,  0.857f,  0.137f,
    1.0f,-1.0f,-1.0f,   0.055f,  0.953f,  0.042f,
    1.0f, 1.0f, 1.0f,   0.714f,  0.505f,  0.345f,
    1.0f,-1.0f, 1.0f,   0.783f,  0.290f,  0.734f,
    1.0f, 1.0f, 1.0f,   0.722f,  0.645f,  0.174f,
    1.0f, 1.0f,-1.0f,   0.302f,  0.455f,  0.848f,
    -1.0f, 1.0f,-1.0f,  0.225f,  0.587f,  0.040f,
    1.0f, 1.0f, 1.0f,   0.517f,  0.713f,  0.338f,
    -1.0f, 1.0f,-1.0f,  0.053f,  0.959f,  0.120f,
    -1.0f, 1.0f, 1.0f,  0.393f,  0.621f,  0.362f,
    1.0f, 1.0f, 1.0f,   0.673f,  0.211f,  0.457f,
    -1.0f, 1.0f, 1.0f,  0.820f,  0.883f,  0.371f,
    1.0f,-1.0f, 1.0f,   0.982f,  0.099f,  0.879f,
  };

  GLuint VBO;
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
   	       g_vertex_buffer_data, GL_STATIC_DRAW);

  // Set up a MVP matrix for the triangle.

  // Model:
  glm::mat4 Model = glm::mat4(1.0f);

  // View / Camera
  glm::mat4 View = glm::lookAt(
			       glm::vec3(4, 3, 3), // camera location
			       glm::vec3(0, 0, 0), // looking at origin
			       glm::vec3(0, 1, 0)  // setting the up direction
			       );

  // Projection
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
					  (float) sizeX / (float) sizeY,
					  0.1f, 100.0f);

  // Set up the shader program and use it.
  GLuint programID = load_shaders(vertexShaderPath, fragmentShaderPath);
  glUseProgram(programID);
  
  // Get uniform and attribute locations
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  GLuint TimeID = glGetUniformLocation(programID, "u_time");

  // position and colour att. locations
  GLint position_attr_i = glGetAttribLocation(programID, "vPosition");
  GLint colour_attr_i = glGetAttribLocation(programID, "vColour");
  
  if (glcontext) {
    std::cout << "\tOpenGLES version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "\tVendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "\tRenderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "\tShading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Set the interval for vsync
    SDL_GL_SetSwapInterval(1);

    // Set the clear colour and depth testing
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
  } else {
    std::cout << "Error: Could not create OpenGLES context" << std::endl;
  }

  // Load up an image to use as a texture
  SDL_Surface* tex_surf = IMG_Load(texturePath);
  
  
  // Don't need the image anymore.
  SDL_FreeSurface(tex_surf);

  bool shouldExit = false;
  SDL_Event event;

  // Declare the uniforms (float time and mvp)
  float float_time;
  glm::mat4 mvp;
   
  while(!shouldExit) {

    while (SDL_PollEvent(&event) != 0) {
      if(event.type == SDL_KEYDOWN) {
	shouldExit = true;
	break;
      }
    }

    // Rotate the mvp every frame.
    Model = glm::rotate(Model, 0.01f, glm::vec3(0.2, 1.0, 0.0));
    mvp = Projection * View * Model;
    
    // Positions here.
    glEnableVertexAttribArray(position_attr_i);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(position_attr_i, 3,
			  GL_FLOAT, GL_FALSE,
    			  6*sizeof(GLfloat), (void*) (0*sizeof(float)));
    
    // Colours here
    glEnableVertexAttribArray(colour_attr_i);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(colour_attr_i, 3,
    			  GL_FLOAT, GL_FALSE,
    			  6*sizeof(GLfloat), (void*) (3*sizeof(float)));   
  
    // Update the mvp + time
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    float_time = (float) SDL_GetTicks();
    glUniform1f(TimeID, float_time);
    
    glDrawArrays(GL_TRIANGLES, 0, 12*3);
    
    SDL_GL_SwapWindow(window);

    glDisableVertexAttribArray(position_attr_i);
    glDisableVertexAttribArray(colour_attr_i);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
  }
  
  // Clean up
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
  
  return EXIT_SUCCESS;

} 
