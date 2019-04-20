#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 1) in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
}