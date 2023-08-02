#version 330 core

uniform float ambientLighting;
uniform vec3 objectColor;
uniform vec3 lightColor;

out vec4 FragColor;

void main()
{
    vec3 ambient = ambientLighting * lightColor;
    vec3 result = ambient * objectColor;

	FragColor = vec4(result, 1.0);
}