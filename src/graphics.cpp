#include <graphics.h>

#include <iostream>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "file_utility.h"

void Shader::Init(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const auto vertexShaderProgram = LoadFile(vertexShaderPath);
	const char* vertexShaderChar = vertexShaderProgram.c_str();

	glShaderSource(vertexShader, 1, &vertexShaderChar, NULL);
	glCompileShader(vertexShader);
	//Check success status of shader compilation 
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return;
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	auto fragmentShaderProgram = LoadFile(fragmentShaderPath);
	const char* fragmentShaderChar = fragmentShaderProgram.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderChar, NULL);
	glCompileShader(fragmentShader);
	//Check success status of shader compilation 
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//Check if shader program was linked correctly
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		return;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Bind()
{
	glUseProgram(shaderProgram);
}
