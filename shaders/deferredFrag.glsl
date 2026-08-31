#version 410 core

out vec4 FragColour;

in vec2 texCords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform float ambient = 0.8;

void main()
{
	vec3 FragPos = texture(gPosition, texCords).rgb;
	vec3 Normal = normalize(texture(gNormal, texCords).rgb);
	vec3 Albedo = texture(gAlbedo, texCords).rgb;

	vec3 LightPos = vec3(FragPos.x, FragPos.y + 1.0, FragPos.z);
	vec3 lightDir = normalize(LightPos - FragPos);

	float diff = max(dot(Normal, lightDir), 0.0f);
	vec3 diffuse = diff * vec3(0.6, 0.59, 0.4);
	vec3 ambientColour = ambient * vec3(0.82, 0.82, 0.52);

	vec3 result = (ambient + diffuse) * Albedo.rgb;
	FragColour = vec4(result, 1.0f);
}