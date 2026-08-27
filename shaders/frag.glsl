#version 410 core

in vec2 texCords;
in vec3 fragPos;
in vec3 Normal;

uniform sampler2D textureSampler;
uniform vec3 lightPos;
uniform float ambient = 0.8;

out vec4 color;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 testLightPos = vec3(fragPos.x, fragPos.y + 1.0, fragPos.z);
	vec3 lightDir = normalize(testLightPos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * vec3(0.6, 0.59, 0.4);
	vec3 ambientColour = ambient * vec3(0.82, 0.82, 0.52);

	vec3 result = (ambient + diffuse) * texture(textureSampler, texCords).rgb;
	color = vec4(result, 1.0f);
}