#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 TexCoords;

layout(binding = 0) uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
}