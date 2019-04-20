#version 450
layout(location = 0) in vec3 aPos;

layout(set = 0, binding = 0) uniform lampVertBlock
{
	mat4 model;
	mat4 view;
	mat4 projection;
};
void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}