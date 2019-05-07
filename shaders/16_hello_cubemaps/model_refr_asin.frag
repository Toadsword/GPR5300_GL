
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform float reflectionValue;
struct Material 
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};
uniform Material material;
uniform float refractiveIndex = 1.52;

vec3 refract_custom(vec3 I,vec3 N,float eta)
{
	vec3 InvertedN = vec3(pow(N.x, -1), pow(N.y, -1), pow(N.z, -1));
	vec3 R = -InvertedN * cos(asin(eta * sin(acos(dot(I, N)))));
	return R;
}

void main()
{    
	float ratio = 1.00 / refractiveIndex;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract_custom(I, normalize(Normal), ratio);

	vec3 reflColor = texture(skybox, R).rgb;
	vec3 diffuseColor = texture(material.texture_diffuse1, TexCoords).rgb; 
    FragColor = vec4(mix(diffuseColor, reflColor, reflectionValue),1.0);
}