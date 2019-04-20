#version 450
layout(location = 0) out vec4 FragColor;
layout(location = 1) in vec2 TexCoord;

layout(binding = 0) uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}