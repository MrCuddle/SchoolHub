#version 330 core

in vec2 texCoordVs;

out vec4 color;

uniform sampler2D tex;

void main()
{
	float depthValue = texture(tex, texCoordVs).r;
	color = vec4(vec3(depthValue), 1.0);
}