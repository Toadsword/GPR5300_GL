
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float heightResolution;
uniform float heightOrigin;

uniform sampler2D texture1;
uniform sampler2D texture3;

void main()
{
	vec4 modelPos = vec4(aPos, 1.0);
	float height = texture(texture1, aTexCoord).r * heightResolution + heightOrigin;
	modelPos.y = height;
	vec4 pos = projection * view * model * modelPos;

	gl_Position = pos;
	TexCoord = aTexCoord;
}