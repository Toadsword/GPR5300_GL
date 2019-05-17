layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform mat4 viewProj;
uniform vec3 position;

void main()
{
	vec3 billboard_position = 
		position 
		+ cameraRight * aPos.x 
		+ cameraUp * aPos.y;

    TexCoords = aTexCoords;
    gl_Position = viewProj * vec4(billboard_position , 1.0);
}