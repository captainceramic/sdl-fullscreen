/* Function to load and compile an OpenGLES shader program. */
#include <stdio.h>
#include <stdlib.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "shader_loader.h"


GLuint load_shaders(const char *vertex_shader_path,
		    const char *fragment_shader_path) {
 
  /* load the vertex shader and fragment shader code */
  FILE *vertex_file = fopen(vertex_shader_path, "rb");
  FILE *fragment_file = fopen(fragment_shader_path, "rb");

  fseek(vertex_file, 0, SEEK_END);
  fseek(fragment_file, 0, SEEK_END);

  long vertex_fsize = ftell(vertex_file);
  long fragment_fsize = ftell(fragment_file);

  rewind(vertex_file);
  rewind(fragment_file);

  char * vertex_shader_source = malloc(vertex_fsize + 1);
  char * fragment_shader_source = malloc(fragment_fsize + 1);

  fread(vertex_shader_source, vertex_fsize, 1, vertex_file);
  fread(fragment_shader_source, fragment_fsize, 1, fragment_file);

  fclose(vertex_file);
  fclose(fragment_file);

  vertex_shader_source[vertex_fsize] = 0;
  fragment_shader_source[fragment_fsize] = 0;
  
  const GLchar * final_vertex_shader_source = vertex_shader_source;
  const GLchar * final_fragment_shader_source = fragment_shader_source;

  /* start compiling shaders */
  enum Consts {INFOLOG_LEN = 512};
  GLchar infoLog[INFOLOG_LEN];
  GLint success;
  
  GLuint fragment_shader;
  GLuint vertex_shader;
  GLuint shader_program;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  printf("vertex shader id is: %d\n", vertex_shader);
  glShaderSource(vertex_shader, 1, &final_vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
  if (!success) {
    printf("ERROR: Vertex Shader compilation failed:\n%s\n", infoLog);
  } else {
    printf("Vertex shader source is:\n%s\n", final_vertex_shader_source);
  }

  /* fragment shader */
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  printf("fragment shader id is: %d\n", fragment_shader);  
  glShaderSource(fragment_shader, 1, &final_fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
  if (!success) {
    printf("ERROR: Fragment Shader compilation failed:\n%s\n", infoLog);
  } else {
    printf("Fragment shader source is:\n%s\n", final_fragment_shader_source);
  }

  /* link the shaders */
  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);

  /* Add in attributes */
  glBindAttribLocation(shader_program, 0, "aPos");

  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
  if (!success) {
    printf("ERROR: Shader Program failed to link:\n%s\n", infoLog);
  } 

  /* Clean up and delete */
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  free(vertex_shader_source);
  free(fragment_shader_source);
  
  return shader_program;
  
}
