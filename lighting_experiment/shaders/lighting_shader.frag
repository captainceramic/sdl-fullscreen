#version 100

precision mediump float;

uniform vec3 objectColour;
uniform vec3 lightColour;

void main() {
  gl_FragColor = vec4(lightColour * objectColour, 1.0);
}
