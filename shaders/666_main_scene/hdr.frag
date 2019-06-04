out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform float exposure = 1.0;
void main()
{        
	const float gamma = 2.2;
    vec4 hdrColor = texture(hdrBuffer, TexCoords).rgba;	
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += vec4(bloomColor, 0.0f); // additive blending
	
	if(bloomColor.r > 0.99 && bloomColor.g > 0.99 && bloomColor.b > 0.99)
		discard; 

	float a = 3 - bloomColor.r - bloomColor.g - bloomColor.b;

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor.rgb * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));

	
	FragColor = gl_FragColor + vec4(result, a);
}