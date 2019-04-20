#version 450
layout (location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform lampFragBlock
{
	vec3 lightColor;
};
void main()
{
	FragColor = vec4(lightColor,0.0);
}