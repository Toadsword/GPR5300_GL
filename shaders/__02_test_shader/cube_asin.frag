
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	average = asin(average);
    FragColor = vec4(average, average, average, 1.0);
}   