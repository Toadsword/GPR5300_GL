#version 330 core
struct EngineMaterial 
{
	sampler2D texture_diffuse1;
	sampler2D texture_normal1;
	sampler2D texture_specular1;
	float shininess;
};

struct VS_OUT 
{
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
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

vec3 calculate_point_light(EnginePointLight light, VS_OUT fs_in, EngineMaterial material, vec3 normal)
{

	vec3 tangentLightPos = fs_in.TBN * light.position;
	float distance    = length(light.position - fs_in.TangentFragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance)); 

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(tangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));

	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));

	return light.intensity * attenuation * (diffuse + specular);
}

struct EngineDirectionLight 
{
    vec3 direction;
	float intensity;
};

vec3 calculate_directional_light(EngineDirectionLight light, VS_OUT fs_in, EngineMaterial material, vec3 normal)
{
 // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));
        
    return light.intensity * (diffuse + specular);
}

struct EngineSpotLight {
	vec3 position;
	vec3  direction;
	float cutOff;
	float outerCutOff;
	float intensity;
};

vec3 calculate_spot_light(EngineSpotLight light, VS_OUT fs_in, EngineMaterial material, vec3 normal)
{
	vec3 tangentLightPos = fs_in.TBN * light.position;
	vec3 lightDir = normalize(tangentLightPos - fs_in.TangentFragPos);
  	float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
	     
	// diffuse 
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse =  diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    
	// specular
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular =  spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));
    return light.intensity * intensity * (diffuse + specular);
}

const int MAX_POINT_LIGHT = 5;
const int MAX_SPOT_LIGHT = 5;

uniform EnginePointLight pointLights[MAX_POINT_LIGHT];
uniform EngineSpotLight spotLights[MAX_SPOT_LIGHT];
uniform EngineDirectionLight directionLight;
uniform int pointLightsNmb = 0;
uniform int spotLightsNmb = 0;
uniform bool directionalLightEnable = false;
uniform float ambientIntensity = 0.2;

