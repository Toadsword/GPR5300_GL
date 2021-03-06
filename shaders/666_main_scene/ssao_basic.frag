layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gAlbedoSSAO;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    gAlbedo.rgb = vec3(texture(texture_diffuse1, TexCoords));
    gAlbedoSSAO.rgb = gAlbedo.rgb;
	
	if(gAlbedo.r > 0.01 && gAlbedo.g > 0.01 && gAlbedo.b > 0.01)
		discard;
}