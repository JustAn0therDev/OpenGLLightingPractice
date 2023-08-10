#version 330 core

uniform float ambientLighting;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

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

	// calculate specular lighting
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
    // Default shininess of the material being 32 (can vary from 2 to 256, 1 bit)
    float shininess = 256;
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

	FragColor = vec4(result, 1.0);
}