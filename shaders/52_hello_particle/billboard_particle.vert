
// Input vertex data, different for all executions of this shader
layout(location = 3) in vec3 aPos;
layout(location = 4) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 5) in vec4 color; // Position of the center of the particule and size of the square

out vec2 TexCoords;
out vec4 ParticleColor;

uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform mat4 VP;

void main()
{
	float particleSize = xyzs.w; // because we encoded it this way.

	vec3 calculated = xyzs.xyz + CameraRight * aPos.x  * particleSize + CameraUp * aPos.y * particleSize;

    gl_Position = VP * vec4(calculated, 1.0);

    TexCoords = aPos.xy + vec2(0.5, 0.5);
    ParticleColor = color;
}