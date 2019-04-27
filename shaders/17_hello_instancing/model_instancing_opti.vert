
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 translatePos;

out vec2 TexCoords;
out vec3 Normals;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float asteroidScale;

void main()
{ 
	mat4 instanceMatrix = identity();
	instanceMatrix = instanceMatrix * translate(translatePos.x, translatePos.y, translatePos.z);
	instanceMatrix = instanceMatrix * scale(asteroidScale, asteroidScale, asteroidScale);
    TexCoords = aTexCoords;    
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}