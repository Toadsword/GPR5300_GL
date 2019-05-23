
layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoords) - ParticleColor;
    if(texColor.a < 0.01)
        discard;
    FragColor = texColor;
}