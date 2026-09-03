#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 textureCords;
layout(location=2) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 perspective;
uniform mat4 view;

out vec2 texCords;
out vec3 fragPos;
out vec3 Normal;

void main()
{
	texCords = textureCords;
	fragPos = vec3(view * modelMatrix * vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(view * modelMatrix))) * normal;

	vec4 newPosition = perspective * view * modelMatrix * vec4(position, 1.0f);

	gl_Position = newPosition;
}