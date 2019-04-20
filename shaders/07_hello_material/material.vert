#version 450
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

layout(set = 0, binding = 0) uniform lightVertBlock
{
	mat4 model;
	mat4 view;
	mat4 projection;
};
layout(location = 2) out vec3 FragPos;
layout(location = 3) out vec3 Normal;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
}