layout (location = 0) in vec3 aPos;
layout (location = 5) in mat4 aModel;

uniform mat4 lightSpaceMatrix;
//uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * aModel * vec4(aPos, 1.0);
}  