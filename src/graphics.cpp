#include <graphics.h>

#include <iostream>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <gli/gli.hpp>
#include <glm/glm.hpp>
#include "file_utility.h"

void Shader::Init(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
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

	const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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

int Shader::GetProgram()
{
	return shaderProgram;
}

unsigned CreateTexture(char const* filename)
{
	gli::texture Texture = gli::load(filename);
	if (Texture.empty())
		return 0;

	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
	GLenum Target = GL.translate(Texture.target());

	GLuint TextureName = 0;
	glGenTextures(1, &TextureName);
	glBindTexture(Target, TextureName);
	glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

	glm::tvec3<GLsizei> const Extent(Texture.extent());
	GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

	switch (Texture.target())
	{
	case gli::TARGET_1D:
		glTexStorage1D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x);
		break;
	case gli::TARGET_1D_ARRAY:
	case gli::TARGET_2D:
	case gli::TARGET_CUBE:
		glTexStorage2D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Extent.x, Texture.target() == gli::TARGET_2D ? Extent.y : FaceTotal);
		break;
	case gli::TARGET_2D_ARRAY:
	case gli::TARGET_3D:
	case gli::TARGET_CUBE_ARRAY:
		glTexStorage3D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Extent.x, Extent.y,
			Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
		break;
	default:
		assert(0);
		break;
	}

	for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
		for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
			for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				GLsizei const LayerGL = static_cast<GLsizei>(Layer);
				glm::tvec3<GLsizei> Extent(Texture.extent(Level));
				Target = gli::is_target_cube(Texture.target())
					? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
					: Target;

				switch (Texture.target())
				{
				case gli::TARGET_1D:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage1D(
							Target, static_cast<GLint>(Level), 0, Extent.x,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage1D(
							Target, static_cast<GLint>(Level), 0, Extent.x,
							Format.External, Format.Type,
							Texture.data(Layer, Face, Level));
					break;
				case gli::TARGET_1D_ARRAY:
				case gli::TARGET_2D:
				case gli::TARGET_CUBE:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage2D(
							Target, static_cast<GLint>(Level),
							0, 0,
							Extent.x,
							Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage2D(
							Target, static_cast<GLint>(Level),
							0, 0,
							Extent.x,
							Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
							Format.External, Format.Type,
							Texture.data(Layer, Face, Level));
					break;
				case gli::TARGET_2D_ARRAY:
				case gli::TARGET_3D:
				case gli::TARGET_CUBE_ARRAY:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage3D(
							Target, static_cast<GLint>(Level),
							0, 0, 0,
							Extent.x, Extent.y,
							Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage3D(
							Target, static_cast<GLint>(Level),
							0, 0, 0,
							Extent.x, Extent.y,
							Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
							Format.External, Format.Type,
							Texture.data(Layer, Face, Level));
					break;
				default: assert(0); break;
				}
			}
	return TextureName;
}

const std::string& DrawingProgram::GetProgramName()
{
	return programName;
}

const std::vector<Shader*>& DrawingProgram::GetShaders()
{
	return shaders;
}
