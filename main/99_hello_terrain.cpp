#include <engine.h>
#include <graphics.h>

#ifdef _DEBUG
#include <iostream>
#endif

#include <imgui.h>

#include <gli/gli.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifdef USE_SFML2
#include "SFML/Graphics/Texture.hpp"
#endif
class HelloTerrainDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void ProcessInput();
	void Destroy() override;
	void UpdateUi() override;
private:
	Shader shaderProgram;
	unsigned VBO[2] = {};
	unsigned int VAO = 0;
	unsigned int EBO = 0;


	unsigned terrainTexture = 0;
	unsigned terrainHeightMap = 0;
#ifdef USE_SFML2
	sf::Texture sfTerrainTexture;
	sf::Texture sfTerrainHeightMap;
#endif
	float* vertices = nullptr;
	float* texCoords = nullptr;
	unsigned int* indices = nullptr;

	float terrainOriginY = -1.0f;
	float terrainElevationFactor = 5.0f;

	const size_t terrainWidth = 512l;
	const size_t terrainHeight = 512l;
	const float terrainResolution = 0.1f;

	const size_t verticesCount = terrainWidth * terrainHeight;
	const size_t faceCount = 2 * (terrainWidth - 1) * (terrainHeight - 1);

};

void HelloTerrainDrawingProgram::Init()
{
	programName = "HelloTerrain";

	vertices = (float*)calloc(3*verticesCount, sizeof(float));//vec3, so 3 floats
	texCoords = (float*)calloc(2*verticesCount, sizeof(float));//vec2, so 2 floats

	for (size_t i = 0l; i < verticesCount; i++)
	{
		vertices[3 * i] = -(float)terrainWidth * terrainResolution / 2.0f + (float)(i % terrainWidth) * terrainResolution;//x
		vertices[3 * i + 1] = 0.0f;//y
		vertices[3 * i + 2] = -(float)terrainHeight * terrainResolution / 2.0f + (float)(i / terrainWidth) * terrainResolution;//z

#ifdef _DEBUG
        //std::cout << "Vertex: " << i << " x: "<<vertices[3 * i]<<" y:"<< vertices[3 * i + 2] <<"\n";
#endif
    }
	for (size_t i = 0l; i < verticesCount; i++)
	{
	    const float width = terrainWidth;
	    const float height = terrainHeight;
		texCoords[2 * i] = (float)((i % terrainWidth)+1) / (width+1);
        texCoords[2 * i + 1]  = (float)((i / terrainWidth)+1) / (height+1);
#ifdef _DEBUG
        //std::cout << "TexCoords: " << i << " x: "<<texCoords[2 * i] <<" y:"<< texCoords[2 * i + 1] <<"\n";
#endif
	}
	indices = (unsigned *)calloc(3l * faceCount, sizeof(unsigned));
	size_t quad = 0;
	for(size_t y = 0; y < terrainHeight-1;y++)
    {
	    for(size_t x = 0; x < terrainWidth-1;x++)
        {
	        const unsigned origin = x + y*terrainWidth;
	        const unsigned originBottom = origin+terrainWidth;
	        //face1
	        indices[6*quad] = origin;
	        indices[6*quad+1] = origin+1;
	        indices[6*quad+2] = originBottom;
#ifdef _DEBUG
            //std::cout << "Indices "<<6*quad<<" 1: "<<indices[6*quad] <<" 2: "<< indices[6*quad+1]  <<" 3: " <<indices[6*quad+2]<<"\n";
#endif
	        //face2
            indices[6*quad+3] = origin+1;
            indices[6*quad+4] = originBottom+1;
            indices[6*quad+5] = originBottom;
#ifdef _DEBUG
            //std::cout << "Indices "<<6*quad+3<<" 1: "<<indices[6*quad+3]<<" 2: "<< indices[6*quad+4]  <<" 3: " <<indices[6*quad+5]<<"\n";
#endif
            quad++;
        }
    }

    shaderProgram.CompileSource("shaders/99_hello_terrain/terrain.vert", "shaders/99_hello_terrain/terrain.frag");
	shaders.push_back(&shaderProgram);
#ifdef USE_SFML2
	sfTerrainHeightMap.loadFromFile("data/terrain/terrain_height2048.png");
	sfTerrainHeightMap.setSmooth(true);
	terrainHeightMap = sfTerrainHeightMap.getNativeHandle();

	sfTerrainTexture.loadFromFile("data/terrain/terrain_texture2048.png");
	sfTerrainTexture.setSmooth(true);
	terrainTexture = sfTerrainTexture.getNativeHandle();
#else
	terrainHeightMap = stbCreateTexture("data/terrain/plains_height.png",true, false);
	terrainTexture = stbCreateTexture("data/terrain/plains_texture.png", true, false);
#endif

	glGenBuffers(2, &VBO[0]);
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO); //like: new VAO()
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);//Now use our VAO
	//bind vertices data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * 3* sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//bind texture coords data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	//bind vertices index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * sizeof(unsigned), indices, GL_STATIC_DRAW);
	//unbind Vertex Array
	glBindVertexArray(0);
}

void HelloTerrainDrawingProgram::Draw()
{

	ProcessInput();

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / config.screenHeight, 0.1f, 1000.0f);

	shaderProgram.Bind();
	const int viewLoc = glGetUniformLocation(shaderProgram.GetProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	const int projectionLoc = glGetUniformLocation(shaderProgram.GetProgram(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	const int modelLoc = glGetUniformLocation(shaderProgram.GetProgram(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	const int heightFactorConstLoc = glGetUniformLocation(shaderProgram.GetProgram(), "heightResolution");
	glUniform1f(heightFactorConstLoc, terrainElevationFactor);
    const int heightConstLoc = glGetUniformLocation(shaderProgram.GetProgram(), "heightOrigin");
    glUniform1f(heightConstLoc, terrainOriginY);
	glUniform1i(glGetUniformLocation(shaderProgram.GetProgram(), "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram.GetProgram(), "texture2"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainHeightMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrainTexture);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void HelloTerrainDrawingProgram::ProcessInput()
{
	Engine* engine = Engine::GetPtr();
	auto& inputManager = engine->GetInputManager();
	auto& camera = engine->GetCamera();
	float dt = engine->GetDeltaTime();
	float cameraSpeed = 1.0f;

#ifdef USE_SDL2
	if (inputManager.GetButton(SDLK_w))
	{
		camera.ProcessKeyboard(FORWARD, engine->GetDeltaTime());
	}
	if (inputManager.GetButton(SDLK_s))
	{
		camera.ProcessKeyboard(BACKWARD, engine->GetDeltaTime());
	}
	if (inputManager.GetButton(SDLK_a))
	{
		camera.ProcessKeyboard(LEFT, engine->GetDeltaTime());
	}
	if (inputManager.GetButton(SDLK_d))
	{
		camera.ProcessKeyboard(RIGHT, engine->GetDeltaTime());
	}
#endif

	const auto mousePos = inputManager.GetMousePosition();

	camera.ProcessMouseMovement(mousePos.x, mousePos.y, true);

	camera.ProcessMouseScroll(inputManager.GetMouseWheelDelta());
}


void HelloTerrainDrawingProgram::Destroy()
{
	free(vertices);
	free(texCoords);
	free(indices);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(1, &EBO);
}

void HelloTerrainDrawingProgram::UpdateUi() 
{
	DrawingProgram::UpdateUi();
	ImGui::Separator();
	ImGui::SliderFloat("Terrain Height Mult", &terrainElevationFactor, -10.0f, 10.0f, "height = %.3f");
    ImGui::SliderFloat("Terrain Height Origin", &terrainOriginY, -10.0f, 10.0f, "height = %.3f");
}

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Terrain";

	engine.AddDrawingProgram(new HelloTerrainDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}
