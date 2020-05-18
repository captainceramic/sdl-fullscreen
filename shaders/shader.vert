#version 100

attribute vec3 vPosition;
attribute vec3 vColour;
attribute vec2 vTexCoord;

uniform mat4 MVP;

varying vec3 fragmentColour;
varying vec2 texCoord;

void main() {

  fragmentColour = vColour;
  texCoord = vTexCoord;
  gl_Position = MVP * vec4(vPosition, 1.0);
  
}
