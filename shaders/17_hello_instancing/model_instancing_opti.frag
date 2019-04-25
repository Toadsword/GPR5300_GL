
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normals;

struct Material 
{
	sampler2D texture_diffuse1;
};

uniform Material material;
void main()
{    
    FragColor = texture(material.texture_diffuse1, TexCoords);
}