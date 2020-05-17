#version 100

precision mediump float;

uniform float u_time;

varying vec4 fragmentColour;

void main() {
  gl_FragColor = abs(sin(0.001 * u_time)) * fragmentColour;
}
