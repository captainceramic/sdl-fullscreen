#version 100

attribute vec3 vPosition;
uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_projection;

void main() {
  /* gl_Position = mat_model * mat_view * mat_projection * vec4(vPosition, 1.0); */
  gl_Position = vec4(vPosition, 1.0);
}
