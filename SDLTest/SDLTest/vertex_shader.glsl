#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
out vec2 UV;

uniform mat4 MVP;

void main()
{
	vec4 v = vec4(vertexPosition_modelspace,1);
	gl_Position = MVP * v;

	UV = vertexUV;
}