#version 410 core

layout(location=0) out vec3 gPosition;
layout(location=1) out vec3 gNormal;
layout(location=2) out vec4 gAlbedo;

in vec2 texCords;
in vec3 fragPos;
in vec3 Normal;

uniform sampler2D textureSampler;

void main()
{
	gPosition = fragPos;

	gNormal = normalize(Normal);

	gAlbedo = vec4(texture(textureSampler, texCords).rgb, 1.0);
}