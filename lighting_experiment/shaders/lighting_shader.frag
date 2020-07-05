#version 100

precision mediump float;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos;

uniform float ambientStrength;

varying vec3 Normal;
varying vec3 FragPos;

void main() {

  /* Implement ambient lighting */
  vec3 ambient = ambientStrength * lightColour;

  /* Implement diffuse lighting */
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColour;
  
  vec3 result = (ambient + diffuse) * objectColour;
  
  gl_FragColor = vec4(result, 1.0);
}
