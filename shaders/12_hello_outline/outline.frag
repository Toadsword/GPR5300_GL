#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 outlineColor;
void main()
{    
    FragColor = vec4(outlineColor, 1.0);
}