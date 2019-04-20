#version 450
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(set = 0, binding = 0) uniform materialVertBlock
{
	mat4 model;
	mat4 view;
	mat4 projection;
};
layout(location = 3) out vec3 FragPos;
layout(location = 4) out vec3 Normal;
layout(location = 5) out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;
}