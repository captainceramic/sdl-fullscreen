#version 100

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;
uniform mat4 mat_normal;

attribute vec3 vPosition;
attribute vec3 vNormal;

varying vec3 Normal;
varying vec3 FragPos;

void main() {
  gl_Position = perspective * view * model * vec4(vPosition, 1.0);

  /* Pass information to fragment shader */

  /* multiply by the normal matrix */
  Normal = mat3(mat_normal) * vNormal;
  FragPos = vec3(model * vec4(vPosition, 1.0));
  
}
