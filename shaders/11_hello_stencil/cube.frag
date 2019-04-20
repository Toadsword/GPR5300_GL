#version 450
layout(location = 0) out vec4 FragColor;
layout(location = 1) in vec2 TexCoord;

layout(binding = 0) uniform sampler2D ourTexture;
layout(location = 2) uniform vec3 overrideColor;


void main()
{
    FragColor = texture(ourTexture, TexCoord) * vec4(overrideColor,1.0);
}