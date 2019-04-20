#version 450
layout (location = 0) out vec4 FragColor;

layout (location = 1) in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;
layout(binding = 1) uniform sampler2D depthTexture;
layout(location = 2) uniform vec3 fogColor;
layout(location = 3) uniform float zFar;


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