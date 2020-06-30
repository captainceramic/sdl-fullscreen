#version 100

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

attribute vec3 vPosition;

void main() {
  gl_Position = perspective * view * model * vec4(vPosition, 1.0);
}
