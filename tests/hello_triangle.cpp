/*
MIT License

Copyright (c) 2017 SAE Institute Switzerland AG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <engine.h>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <graphics.h>

//#define EBO_DOUBLE_TRIANGLE

class HelloTriangleDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
private:

#ifndef EBO_DOUBLE_TRIANGLE
	float vertices[9] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
#else
	float vertices[12] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[6] = {
		// note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	unsigned int EBO; // Element Buffer Object
#endif

	unsigned int VBO; //Vertex Buffer Object
	unsigned int VAO; //Vertex Array Object
	Shader shaderProgram;

};

void HelloTriangleDrawingProgram::Init()
{
	glGenBuffers(1, &VBO);
#ifdef EBO_DOUBLE_TRIANGLE
	glGenBuffers(1, &EBO);
#endif
	
	shaderProgram.Init("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &VAO);
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

#ifdef EBO_DOUBLE_TRIANGLE

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#endif
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
}

void HelloTriangleDrawingProgram::Draw()
{
	shaderProgram.Bind();

	glBindVertexArray(VAO);
#ifndef EBO_DOUBLE_TRIANGLE
	glDrawArrays(GL_TRIANGLES, 0, 3);
#else
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
#endif
}

int main()
{
	Engine engine;

	engine.AddDrawingProgram(new HelloTriangleDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}