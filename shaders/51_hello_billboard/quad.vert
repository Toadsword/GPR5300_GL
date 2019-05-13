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
    TexCoords = aTexCoord;

	vec3 vertexPosition_worldspace = 
		CameraRight * aPos.x
		+ CameraUp * aPos.y + aPos.z;


    //ParticleColor = color;
    gl_Position = projection * vec4((aPos) + offset, 1.0);
}