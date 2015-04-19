#version 410 core

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec4 inPosition;

void main()
{
	gl_Position = projectionMatrix * modelViewMatrix * inPosition;
}