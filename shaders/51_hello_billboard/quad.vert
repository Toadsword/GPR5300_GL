layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoords;
//out vec4 ParticleColor;

uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform mat4 projection;
uniform vec3 offset;
//uniform vec4 color;

void main()
{
	vec3 calculated = offset + CameraRight * aPos.x + CameraUp * aPos.y;

	float scale = 10.0f;
    TexCoords = aTexCoord;
//    ParticleColor = color;
    gl_Position = projection * vec4(calculated, 1.0);

}