
layout (location = 0) out vec4 FragColor;

in VS_OUT vs_out;

uniform vec3 lightPos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
  
uniform float heightScale;
  
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(depthMap, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * heightScale);
    return texCoords - p;
} 
  
void main()
{
    vec3 TangentLightPos = vs_out.invTBN * lightPos;
    vec3 TangentViewPos  = vs_out.invTBN * vs_out.ViewPos;
    vec3 TangentFragPos  = vs_out.invTBN * vs_out.FragPos;
	
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir   = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = ParallaxMapping(vs_out.TexCoords,  viewDir);
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
		discard;

    // then sample textures with new texture coords
    vec3 normal  = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    
    // get diffuse color
    vec3 color = texture(diffuseMap, texCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}