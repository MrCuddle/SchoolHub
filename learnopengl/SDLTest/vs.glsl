#version 330 core

layout (location = 0) in vec3 position;

void main()
{
	gl_Position.xyz = position.xyz;
	gl_Position.w = 1.0;
}