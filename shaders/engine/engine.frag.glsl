#version 330 core
struct EngineMaterial 
{
	sampler2D texture_diffuse1;
	sampler2D texture_normal1;
	sampler2D texture_specular1;
	float shininess;
};

struct VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
};

struct EnginePointLight
{
	vec3 position;
	float constant;
    float linear;
    float quadratic;
	float intensity;
};

vec3 point_light(EnginePointLight light, VS_OUT fs_in, EngineMaterial material, vec3 normal, vec3 viewPos)
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));

	return light.intensity * (diffuse + specular);
}