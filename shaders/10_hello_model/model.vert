#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(location = 3) out vec2 TexCoords;
layout(set = 0, binding = 0) uniform mvpBlock
{
    mat4 model;
    mat4 view;
    mat4 projection;
};
void main()
{
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}