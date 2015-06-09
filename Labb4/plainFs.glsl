#version 330 core
out vec4 color;

struct Light {
	vec3 position;
	vec3 direction;
	float cutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 normalVs;
in vec3 fragPos;
in vec2 texCoordVs;

uniform vec3 viewPos;
uniform sampler2D sampler;
uniform Light light;

void main()
{
	vec3 lightDir = normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));

	if(theta > light.cutOff)
	{
		vec3 ambient = light.ambient * vec3(texture(sampler, texCoordVs));

		vec3 norm = normalize(normalVs);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff * vec3(texture(sampler, texCoordVs));

		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
		vec3 specular = light.specular * spec * vec3(0.6,0.5,0.6);

		float distance = length(light.position - fragPos);
		float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic *(distance * distance));
		diffuse *= attenuation;
		specular *= attenuation;

		color = vec4(ambient + diffuse + specular, 1.0f);
	}
	else
		color = vec4(light.ambient * vec3(0.3,0.3,0.3),1.0f);
	//color = texture(sampler, texCoordVs);
}