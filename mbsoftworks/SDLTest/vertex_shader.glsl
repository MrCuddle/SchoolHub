#version 410 core

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

smooth out vec3 color;

void main()
{
	gl_Position = projectionMatrix * modelViewMatrix * vec4(inPosition,1.0);
	color = inColor;
}