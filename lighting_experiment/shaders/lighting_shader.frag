#version 100

precision mediump float;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float specularStrength;

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

  /* Implement specular lighting */
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);

  float dot_bit = dot(viewDir, reflectDir);
  float max_bit = max(dot_bit, 0.0);
  float spec = pow(max_bit, float(64));
  vec3 specular = specularStrength * spec * lightColour;
  
  vec3 result = (ambient + diffuse + specular) * objectColour;
  
  gl_FragColor = vec4(result, 1.0);
}
