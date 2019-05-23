
// Input vertex data, different for all executions of this shader
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 color; // Position of the center of the particule and size of the square // 3 first = color, last = scale
layout(location = 3) in vec3 scaleGlow; // Position of the center of the particule and size of the square // 3 first = color, last = scale
layout(location = 4) in vec3 offset; // Position of the center of the particule and size of the square (refreshed every tick)

out vec2 TexCoords;
out vec4 ParticleColor;

uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform mat4 VP;
uniform float time;

void main()
{
	float scale = scaleGlow.x + sin(time * scaleGlow.z) * scaleGlow.y;
	vec3 calculated = offset + CameraRight * aPos.x * scale + CameraUp * aPos.y * scale;

    gl_Position = VP * vec4(calculated, 1.0);

    TexCoords = aTexCoord;
    ParticleColor = vec4(color, 0.0);
}