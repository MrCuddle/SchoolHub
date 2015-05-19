#version 330 core
out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texSampler;
uniform sampler2D normalSampler;

in vec3 normalVs;
in vec3 fragPos;
in vec2 texCoordVs;
in vec3 tangentVs;

vec3 calcBumpedNormal()
{
	vec3 tangent = normalize(tangentVs);
	tangent = normalize(tangent - dot(tangent, normalVs) * normalVs);
	vec3 bitangent = cross(tangent, normalVs);
	vec3 bumpedNormal = texture(normalSampler, texCoordVs).xyz;
	bumpedNormal = 2.0 * bumpedNormal - vec3(1.0,1.0,1.0);
	vec3 newNormal;
	mat3 TBN = mat3(tangent, bitangent, normalVs);
	newNormal = TBN * bumpedNormal;
	newNormal = normalize(newNormal);
	return newNormal; 
}
void main()
{
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = calcBumpedNormal();
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrentgh = 0.5f;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrentgh * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	//color = vec4(result, 1.0f);
	color = texture(texSampler, texCoordVs) * vec4(result, 1.0f);
}