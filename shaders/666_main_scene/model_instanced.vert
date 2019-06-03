layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in vec3 aPositions;
layout (location = 6) in vec3 aAngle;

out vec2 TexCoords;
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

uniform float aScale;
uniform mat4 VP; // View Projection

void main()
{
    TexCoords = aTexCoords;    
	
	mat4 model =  translate(aPositions.x, aPositions.y, aPositions.z);
	model = model * RotateX(aAngle.x);
	model = model * RotateY(aAngle.y);
	model = model * RotateZ(aAngle.z);
	//model = model * scale(aScale, aScale, aScale);
    gl_Position = VP * model * vec4(aPos, 1.0);
}