#pragma once
#include <string>

class Shader
{
public:
	void Init(std::string vertexShaderPath, std::string fragmentShaderPath);
	void Bind();
private:
	unsigned int shaderProgram = 0;
};

class DrawingProgram
{
public:
	virtual void Init() = 0;
	virtual void Draw() = 0;
};