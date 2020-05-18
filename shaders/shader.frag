#version 100

precision mediump float;

uniform float u_time;
uniform sampler2D u_texture;

varying vec3 fragmentColour;
varying vec2 texCoord;

void main() {

  gl_FragColor = texture2D(u_texture, texCoord);
  //gl_FragColor = vec4(fragmentColour, 1.0);
  //gl_FragColor = vec4(fragmentColour.x, abs(texCoord.y), abs(texCoord.x), 1.0);
  
}
