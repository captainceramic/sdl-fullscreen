#version 100

precision mediump float;

uniform float u_time;
uniform sampler2D u_texture;

varying vec3 fragmentColour;
varying vec2 texCoord;

void main() {

  gl_FragColor = mix(texture2D(u_texture, texCoord), vec4(fragmentColour, 1.0), 0.5);
  
}
