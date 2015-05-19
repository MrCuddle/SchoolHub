#version 330 core
out vec4 fragColor;

struct DirLight 
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texSampler;
uniform sampler2D normalSampler;
uniform sampler2D shadowSampler;
uniform DirLight dirLight;
uniform float shadowBias;

in vec3 normalVs;
in vec3 fragPos;
in vec2 texCoordVs;
in vec3 tangentVs;
in vec4 fragPosLightSpaceVs;

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

float shadowCalc(vec4 fragPosLightSpace, vec3 normal)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowSampler, projCoords.xy).r;
	float currentDepth = projCoords.z;
	//float bias = max(0.05 * (1.0 - dot(normal, dirLight.direction)), shadowBias);
	float bias = shadowBias;
	float shadow = currentDepth -bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}

void main()
{
	vec3 color = texture(texSampler, texCoordVs).rgb;
	vec3 normal = calcBumpedNormal();
	vec3 lightColor = vec3(1.0);
	vec3 ambient = 0.15 * color;
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir, normal),0.0);
	vec3 diffuse = diff * lightColor;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;
	float shadow = shadowCalc(fragPosLightSpaceVs, normal);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

	fragColor = vec4(lighting, 1.0); 
}