#version 330 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform vec3 fogColor;
uniform float zFar;


float linearize(float depth) {
    float zNear = 0.1;

    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{ 
	vec4 screenColor = texture(screenTexture, TexCoords);
	vec4 depthColor = texture(depthTexture, TexCoords);
	float depthValue = 1.0-linearize(depthColor.r);
	vec4 color = mix(screenColor, vec4(fogColor,1.0), 1.0-(depthValue*depthValue));
    FragColor = color;
}