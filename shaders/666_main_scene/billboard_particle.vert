
// Input vertex data, different for all executions of this shader
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 offset; // Position of the center of the particule and size of the square

out vec2 TexCoords;
//out vec4 ParticleColor;

uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform mat4 VP;

void main()
{
	vec3 calculated = offset + CameraRight * aPos.x + CameraUp * aPos.y;

    gl_Position = VP * vec4(calculated, 1.0);

    TexCoords = aTexCoord;
    //ParticleColor = color;
}