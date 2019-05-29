
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in mat4 aModel;

out vec2 TexCoords;
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

uniform mat4 VP; // View Projection

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = VP * aModel * vec4(aPos, 1.0);
}