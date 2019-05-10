layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT vs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float heightResolution;
uniform float heightOrigin;

uniform sampler2D heightMap;

void main()
{
	vec4 modelPos = vec4(aPos, 1.0);
	float height = texture(heightMap, aTexCoords).r * heightResolution + heightOrigin;
	modelPos.y = height;
	vec4 pos = projection * view * model * modelPos;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    vs_out.invTBN  = mat3(T, B, N);

	vs_out.ViewPos = viewPos;
	vs_out.FragPos = vec3(pos);   
    vs_out.TexCoords = aTexCoords;	
	gl_Position = pos;
}