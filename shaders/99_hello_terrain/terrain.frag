#version 450
layout(location = 0) out vec4 FragColor;
layout(location = 1) in vec2 TexCoord;

layout(location = 2) uniform sampler2D texture2;

void main()
{
	FragColor = texture(texture2, TexCoord);
}