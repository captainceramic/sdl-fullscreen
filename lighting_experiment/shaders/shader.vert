#version 100

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

attribute vec3 vPosition;
attribute vec3 vNormal;

varying vec3 Normal;
varying vec3 FragPos;

void main() {
  gl_Position = perspective * view * model * vec4(vPosition, 1.0);

  /* Pass information to fragment shader */
  Normal = vNormal;
  FragPos = vec3(model * vec4(vPosition, 1.0));
  
}
