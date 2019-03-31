#pragma once
#include <string>
#include <vector>


class Shader
{
public:
	void Init(std::string vertexShaderPath, std::string fragmentShaderPath);
	void Bind();
	int GetProgram();
private:
	int shaderProgram = 0;
};

class DrawingProgram
{
public:
	virtual ~DrawingProgram() = default;
	virtual void Init() = 0;
	virtual void Draw() = 0;
	virtual void Destroy() = 0;
	virtual void UpdateUi() {};
	const std::string& GetProgramName();
	const std::vector<Shader*>& GetShaders();

protected:
	std::vector<Shader*> shaders;
	std::string programName;
};


unsigned int gliCreateTexture(char const* filename);
unsigned int stbCreateTexture(const char* filename, bool smooth = true, bool mipMaps = true);