#version 330 core

in vec3 vertexColor;
in vec2 vertexTexCoord;

out vec4 color;

uniform sampler2D texSampler;

void main()
{
	color = texture(texSampler, vertexTexCoord);
}