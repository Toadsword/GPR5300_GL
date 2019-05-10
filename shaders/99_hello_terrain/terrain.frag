layout(location = 0) out vec4 FragColor;

in VS_OUT vs_out;

uniform sampler2D diffuseMap;

void main()
{
	FragColor = texture(diffuseMap, vs_out.TexCoords);
}