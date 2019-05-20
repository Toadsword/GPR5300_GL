
layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoords);
    if(texColor.a < 0.01)
        discard;
    FragColor = texColor;
}