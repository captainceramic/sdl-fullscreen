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


const char* teapotPath = "../data/cube.obj";
//const char* teapotPath = "../data/newell_teaset/teapot.obj";

const unsigned int sizeX = 1920;
const unsigned int sizeY = 1080;

const char* fragmentShaderPath = "shaders/shader.frag";
const char* vertexShaderPath = "shaders/shader.vert";

// Function prototype for the model loader
bool loadOBJ(
	     const char* path,
	     std::vector < glm::vec3 > & out_vertices,
	     std::vector < glm::vec2 > & out_uvs,
	     std::vector < glm::vec3 > & out_normals
	     ) {

  // set up temporary variables
  std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
  std::vector< glm::vec3 > temp_vertices;
  std::vector< glm::vec2 > temp_uvs;
  std::vector< glm::vec3 > temp_normals;

  // open up the object file
  FILE * file = fopen(path, "r");
  if( file == NULL ) {
    std::cout << "Impossible to open file!" << std::endl;
    return false;
  }

  while( 1 ) {
    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);

    if (res == EOF)
      break;

    // vertice, uvs and normals first
    if ( strcmp( lineHeader, "v" ) == 0) {
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
      temp_vertices.push_back(vertex);
    } else if ( strcmp( lineHeader, "vt" ) == 0 ){
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y );
      temp_uvs.push_back(uv);
    } else if ( strcmp( lineHeader, "vn" ) == 0) {
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    } else if (strcmp( lineHeader, "f" ) == 0 ) {
      // Now the indexes of each point
      std::string vertex1, vertex2, vertex3;
      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
			   &vertexIndex[0], &uvIndex[0], &normalIndex[0],
			   &vertexIndex[1], &uvIndex[1], &normalIndex[1],
			   &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
      if (matches != 9) {
	std::cout << "File can't be read! line is:\n" << file << std::endl;
	return false;
      }

      // Add the indices of each point to the input ectors
      vertexIndices.push_back(vertexIndex[0]);
      vertexIndices.push_back(vertexIndex[1]);
      vertexIndices.push_back(vertexIndex[2]);

      uvIndices.push_back(uvIndex[0]);
      uvIndices.push_back(uvIndex[1]);
      uvIndices.push_back(uvIndex[2]);

      normalIndices.push_back(normalIndex[0]);
      normalIndices.push_back(normalIndex[1]);
      normalIndices.push_back(normalIndex[2]);
      
    }
  }

  std::cout << "We need to store "
	    << vertexIndices.size()
	    << " vertices." << std::endl;
  
  // Now we replace the indices with the actual values
  for( unsigned int i=0; i<vertexIndices.size(); i++) {
    unsigned int vertexIndex = vertexIndices[i];
    
    // OBJ file indexes from 1 not 0;
    glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
    out_vertices.push_back(vertex);
    
    glm::vec2 uv = temp_uvs[ vertexIndex-1 ];
    out_uvs.push_back(uv);
    
    glm::vec3 normal = temp_normals[ vertexIndex-1];
    out_normals.push_back(normal);
    
  }
  
  return true;
};


int main(int argc, char* argv[]) {

  // variables to store the data
  std::vector< glm::vec3 > vertices;
  std::vector< glm::vec2 > uvs;
  std::vector< glm::vec3 > normals;
  
  if( loadOBJ(teapotPath, vertices, uvs, normals) ) {
    std::cout << "Teapot time!!" << std::endl;
  } else {
    std::cout << "No teapot!!" << std::endl;
  }

  printf("output vertices has %d elements\n", vertices.size());
  for(uint i = 0; i < vertices.size(); i++) {
    printf("%f %f %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
  }
  
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

  // Create a VBO for the teapot vertex data.
  GLuint vertexVBO;
  glGenBuffers(1, &vertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
	       &vertices[0], GL_STATIC_DRAW);

  // MVP matrix for the triangle
  glm::mat4 Model = glm::mat4(1.0f);
  glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.5f, 10.0f), // camera location
			       glm::vec3(0.0, 0.0, 0.0), // looking at origin
			       glm::vec3(0.0, 1.0, 0.0)  // setting up direction
			       );

  glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
					  (float) sizeX / (float) sizeY,
					  0.1f, 100.0f);

  // Set up the shader programs
  GLuint programID = load_shaders(vertexShaderPath, fragmentShaderPath);
  glUseProgram(programID);

  // Get uniform and attribute locations
  GLuint MVP_id = glGetUniformLocation(programID, "MVP");

  GLint position_attr_i = glGetAttribLocation(programID, "vPosition");

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

    // Do some rotations if necessary.
    //Model = glm::rotate(Model, 0.01f, glm::vec3(1.0, 0.2, 0.1));
    mvp = Projection * View * Model;
    
    // Bind the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glVertexAttribPointer(position_attr_i,
			  3,
			  GL_FLOAT,
			  GL_FALSE,
			  0,
			  (void*) (0*sizeof(GLfloat)));
    glEnableVertexAttribArray(position_attr_i);
    
    // Insert the MVP
    glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &mvp[0][0]);
    
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);

    SDL_GL_SwapWindow(window);

    glDisableVertexAttribArray(position_attr_i);

  }


  // Clean up
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return EXIT_SUCCESS;

}
