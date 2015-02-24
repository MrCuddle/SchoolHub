#version 410 core

in vec2 UV;
out vec4 color;

struct DirectionalLight
{
	vec3 color;
	float ambientIntensity;
};

uniform sampler2D myTextureSampler;
uniform DirectionalLight dirLight;

void main()
{
	vec2 flippedTexCoord = vec2(UV.x, 1 - UV.y);
	color = texture(myTextureSampler,flippedTexCoord) * vec4(dirLight.color, 1.0f) * dirLight.ambientIntensity;
}

//void main()
//{
//	vec2 flippedTexCoord = vec2(UV.x, 1 - UV.y);
//	color = texture(myTextureSampler,flippedTexCoord).rgb;
//}