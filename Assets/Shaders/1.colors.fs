#version 330 core

uniform float ambientLighting;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
	// normalize the vectors
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	// calculate diffuse lighting
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// calculate ambient lighting
    vec3 ambient = ambientLighting * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;

	FragColor = vec4(result, 1.0);
}