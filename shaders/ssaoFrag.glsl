#version 410 core
out float FragColor;

in vec2 TexCords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

const vec2 noiseScale = vec2(640.0/4.0, 480.0/4.0); 

uniform mat4 perspective;
uniform mat4 view;

void main()
{
	vec3 fragPos = texture(gPosition, TexCords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = perspective * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        float sampleDepth = texture(gPosition, offset.xy).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth)); // smooth step will smoothly interpolate between 0 and 1 if the depth value falls inside of radius
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;      
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}