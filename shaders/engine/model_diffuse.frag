out vec4 FragColor;

in VS_OUT vs_out;

uniform EngineMaterial material;
uniform vec3 viewPos;

void main()
{
    // obtain normal from normal map in range [0,1]
    vec3 normal= texture(material.texture_normal1, vs_out.TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space



    // get diffuse color
    vec3 color = texture(material.texture_diffuse1, vs_out.TexCoords).rgb;
    // ambient
    vec3 ambient = ambientIntensity * color;
    vec3 lightColor = vec3(0,0,0);

    FragColor = vec4(ambient + lightColor, 1.0);
}