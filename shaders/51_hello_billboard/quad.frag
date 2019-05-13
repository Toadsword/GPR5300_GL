
layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoords) * ParticleColor;
}  