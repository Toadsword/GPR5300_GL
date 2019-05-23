
layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;
in vec3 ParticleColor;

uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoords) - vec4(ParticleColor, 0.0f);
    if(texColor.a < 0.01)
        discard;
    FragColor = texColor;
}