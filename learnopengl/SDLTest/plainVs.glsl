#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3 ) in vec3 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 normalVs;
out vec3 fragPos;
out vec2 texCoordVs;
out vec3 tangentVs;
out vec4 fragPosLightSpaceVs;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	normalVs = mat3(transpose(inverse(model))) * normal;
	fragPos = vec3(model * vec4(position, 1.0f));
	texCoordVs = vec2(texCoord.x, 1.0f - texCoord.y);
	tangentVs = (model * vec4(tangent, 0.0)).xyz;
	fragPosLightSpaceVs = lightSpaceMatrix * vec4(fragPos, 1.0);
}