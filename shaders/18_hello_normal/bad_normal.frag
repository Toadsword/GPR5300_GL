layout (location = 0) out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;


void main()
{           
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // get diffuse color
    vec3 color = texture(diffuseMap, TexCoords).rgb;
    // ambient
    vec3 ambient = 0.2 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}