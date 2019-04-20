#version 450
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 0) uniform lightFragBlock
{
	vec3 objectColor;
	vec3 lightColor;
	float ambientStrength;
	float diffuseStrength;
	float specularStrength;
	int specularPow;
	vec3 lightPos;
	vec3 viewPos;
};
layout(location = 1) in vec3 FragPos;
layout(location = 2) in vec3 Normal;

void main()
{
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(diff*dot(viewDir, reflectDir), 0.0), specularPow);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}