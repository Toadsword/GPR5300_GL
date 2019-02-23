#include <engine.h>
#include <graphics.h>
#include "GL/glew.h"
#include <SFML/OpenGL.hpp>

class HelloTextureDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
private:

	float vertices[12] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	float texCoords[8] = {
		1.0f, 1.0f,	  // top right
		1.0f, 0.0f,   // bottom right
		0.0f, 0.0f,   // bottom left
		0.0f, 1.0f,   // bottom left
	};
	unsigned int indices[6] = {
		// note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	Shader shaderProgram;
	unsigned int VBO[2] = {}; //Vertex Buffer Object
	unsigned int VAO = 0; //Vertex Array Object
	unsigned int EBO = 0; // Element Buffer Object
	GLuint texture;
};

void HelloTextureDrawingProgram::Init()
{
	programName = "HelloTexture";

	shaders.push_back(&shaderProgram);

	glGenBuffers(2, &VBO[0]);
	glGenBuffers(1, &EBO);

	shaderProgram.Init(
		"shaders/texture_vertex.glsl",
		"shaders/texture_fragment.glsl");
	texture = CreateTexture("data/sprites/wall.dds");

	glGenVertexArrays(1, &VAO);
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	//bind vertices data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//bind texture coords data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	//bind vertices index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void HelloTextureDrawingProgram::Draw()
{
	shaderProgram.Bind();
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


int main()
{
	Engine engine;
	engine.AddDrawingProgram(new HelloTextureDrawingProgram());
	engine.Init();
	engine.GameLoop();
	return EXIT_SUCCESS;
}
