#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));

	// This should not be done here, since this operation
	// is costly. That said, there is currently no way to do this outside
	// of the shader, since the normal data is hard-coded.

	Normal = mat3(transpose(inverse(model))) * aNormal;
}