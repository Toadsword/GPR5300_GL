#version 450
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 2) out vec2 TexCoord;
layout(set = 0, binding = 0) uniform mvpBlock
{
	mat4 model;
	mat4 view;
	mat4 projection;
};
layout(location = 3) uniform float heightResolution;
layout(location = 4) uniform float heightOrigin;

layout(location = 5) uniform sampler2D texture1;

void main()
{
	vec4 modelPos = vec4(aPos, 1.0);
	float height = texture(texture1, aTexCoord).r * heightResolution + heightOrigin;
	modelPos.y = height;
	vec4 pos = projection * view * model * modelPos;

	gl_Position = pos;
	TexCoord = aTexCoord;
}