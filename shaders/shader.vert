#version 100

attribute vec4 vPosition;
attribute vec4 vColour;

uniform mat4 MVP;

varying vec4 fragmentColour;

void main() {

  fragmentColour = vColour;
  gl_Position = MVP * vPosition;
  
}
