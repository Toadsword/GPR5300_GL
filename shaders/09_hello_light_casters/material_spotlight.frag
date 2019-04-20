#version 450
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct FlashLight {
	vec3 position;
	vec3  direction;
	float cutOff;
	float outerCutOff;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
layout(location = 0) out vec4 FragColor;
layout(set = 0, binding = 0) uniform materialFragBlock
{
	vec3 objectColor;
	vec3 lightColor;
	vec3 viewPos;
	FlashLight light;
};
layout(location = 1) in vec3 FragPos;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec2 TexCoords;


layout(location = 4) uniform Material material;
void main()
{    
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 lightDir = normalize(light.position - FragPos);
  	float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
	     
	// diffuse 
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        
	vec3 result = ambient + intensity*(diffuse + specular);
	FragColor = vec4(result, 1.0);
	
}