#version 450
layout(location = 0) out vec4 FragColor;
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct DirectionLight {
//vec3 position; //no longer needed for directional lights
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
layout(set = 0, binding = 0) uniform materialFragBlock
{
    DirectionLight light;

    vec3 objectColor;
    vec3 lightColor;
    vec3 viewPos;
};
layout(location = 1) in vec3 FragPos;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec2 TexCoords;
layout(location = 4) uniform Material material;


void main()
{    
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}