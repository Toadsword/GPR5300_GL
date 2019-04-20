#version 450
layout(location = 0) out vec4 FragColor;
layout(location = 2) in vec2 TexCoord;

layout(location = 3) uniform sampler2D texture1;
layout(location = 4) uniform sampler2D texture2;

void main()
{
	vec4 p1 = texture(texture1, TexCoord);
	vec4 p2 = texture(texture2, TexCoord);
	FragColor = mix(p1.rgba*p1.a, p2.rgba*p2.a, 0.2);
}