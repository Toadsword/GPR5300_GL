
// Input vertex data, different for all executions of this shader
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 colorScale; // Position of the center of the particule and size of the square // 3 first = color, last = scale
layout(location = 3) in vec3 offset; // Position of the center of the particule and size of the square (refreshed every tick)

out vec2 TexCoords;
out vec3 ParticleColor;

uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform mat4 VP;

void main()
{
	vec3 calculated = offset + CameraRight * aPos.x * colorScale.a + CameraUp * aPos.y * colorScale.a;

    gl_Position = VP * vec4(calculated, 1.0);

    TexCoords = aTexCoord;
    ParticleColor = colorScale.rgb;
}