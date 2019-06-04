layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec3 aPositions;
layout (location = 6) in vec3 aAngle;


out VS_OUT vs_out;
out vec3 out_Normal;
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

uniform float aScale;
uniform mat4 VP; // View Projection

uniform vec3 viewPos;

void main()
{
    //TexCoords = aTexCoords;    
	
	mat4 model =  translate(aPositions.x, aPositions.y, aPositions.z);
	model = model * RotateX(aAngle.x);
	model = model * RotateY(aAngle.y);
	model = model * RotateZ(aAngle.z);
	//aNormal = (vec3(model * vec4(aNormal);
	//model = model * scale(aScale, aScale, aScale);
	
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    vs_out.ViewPos = viewPos;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 B = normalize(normalMatrix * aBitangent);
    
    //Normal = aNormal;
	vs_out.invTBN = mat3(T, B, N);
    gl_Position = VP * model * vec4(aPos, 1.0);

	out_Normal = N;
}