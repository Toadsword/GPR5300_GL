#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout(set = 0, binding = 0) uniform mvpBlock
{
    mat4 model;
    mat4 view;
    mat4 projection;
};
layout(location = 2) out vec2 TexCoord;

void main()
{
    // note that we read the multiplication from right to left
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}