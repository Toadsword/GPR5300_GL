
layout(location = 0) out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;


void main()
{

    // get diffuse color
    vec3 color = texture(diffuseMap, TexCoords).rgb;
    // ambient
    vec3 ambient = 0.2 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, Normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}