#version 450
layout(location = 0) out vec4 FragColor;
layout (location = 2) in vec3 ourColor;

layout(location = 1) uniform float colorCoeff;

void main()
{
    FragColor = vec4(ourColor * colorCoeff, 1.0f) ;
} 