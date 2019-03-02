#include <engine.h>
#include <graphics.h>

#ifdef _DEBUG
#include <iostream>
#endif
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <imgui.h>

#include <gli/gli.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SFML/Graphics/Texture.hpp"

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
	sf::Texture sfTerrainTexture;
	unsigned terrainHeightMap = 0;
	sf::Texture sfTerrainHeightMap;

	float* vertices = nullptr;
	float* texCoords = nullptr;
	unsigned int* indices = nullptr;

	float terrainOriginY = -1.0f;
	float terrainElevationFactor = 5.0f;

	const size_t terrainWidth = 256l;
	const size_t terrainHeight = 256l;
	const float terrainResolution = 0.04f;

	const size_t verticesCount = terrainWidth * terrainHeight;
	const size_t faceCount = 2 * (terrainWidth - 1) * (terrainHeight - 1);

	glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	float fov = 45.0f;
	float fovScroolSpeed = 100.0f;
	float lastX = 800.0f / 2.0f;
	float lastY = 600.0 / 2.0f;
};

void HelloTerrainDrawingProgram::Init()
{
	programName = "HelloTerrain";
	
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	lastX = config.screenWidth / 2.0f;
	lastY = config.screenHeight / 2.0f;

	vertices = (float*)calloc(3*verticesCount, sizeof(float));//vec3, so 3 floats
	texCoords = (float*)calloc(2*verticesCount, sizeof(float));//vec2, so 2 floats

	for (size_t i = 0l; i < verticesCount; i++)
	{

		vertices[3 * i] = -(float)terrainWidth * terrainResolution / 2.0f + (float)(i % terrainWidth) * terrainResolution;//x
		vertices[3 * i + 1] = 0.0f;//y
		vertices[3 * i + 2] = -(float)terrainHeight * terrainResolution / 2.0f + (float)(i / terrainWidth) * terrainResolution;//z

#ifdef _DEBUG
        std::cout << "Vertex: " << i << " x: "<<vertices[3 * i]<<" y:"<< vertices[3 * i + 2] <<"\n";
#endif
    }
	for (size_t i = 0l; i < verticesCount; i++)
	{
	    const float width = terrainWidth;
	    const float height = terrainHeight;
		texCoords[2 * i] = (float)((i % terrainWidth)+1) / (width+1);
        texCoords[2 * i + 1]  = (float)((i / terrainWidth)+1) / (height+1);
#ifdef _DEBUG
        std::cout << "TexCoords: " << i << " x: "<<texCoords[2 * i] <<" y:"<< texCoords[2 * i + 1] <<"\n";
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
            std::cout << "Indices "<<6*quad<<" 1: "<<indices[6*quad] <<" 2: "<< indices[6*quad+1]  <<" 3: " <<indices[6*quad+2]<<"\n";
#endif
	        //face2
            indices[6*quad+3] = origin+1;
            indices[6*quad+4] = originBottom+1;
            indices[6*quad+5] = originBottom;
#ifdef _DEBUG
            std::cout << "Indices "<<6*quad+3<<" 1: "<<indices[6*quad+3]<<" 2: "<< indices[6*quad+4]  <<" 3: " <<indices[6*quad+5]<<"\n";
#endif
            quad++;
        }
    }

	shaderProgram.Init("shaders/hello_terrain/terrain_vertex.glsl", "shaders/hello_terrain/terrain_fragment.glsl");
	shaders.push_back(&shaderProgram);
	
	sfTerrainHeightMap.loadFromFile("data/terrain/terrain_height2048.png");
	sfTerrainHeightMap.setSmooth(true);
	terrainHeightMap = sfTerrainHeightMap.getNativeHandle();

	sfTerrainTexture.loadFromFile("data/terrain/terrain_texture2048.png");
	sfTerrainTexture.setSmooth(true);
	terrainTexture = sfTerrainTexture.getNativeHandle();
	//terrainHeightMap = CreateTexture("data/terrain/terrain_height2048.dds");

	//terrainTexture = CreateTexture("data/terrain/terrain_texture2048.dds");
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
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glm::mat4 view = glm::mat4(1.0f);
	
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f,0.0f,1.0f));
	
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)config.screenWidth / config.screenHeight, 0.1f, 1000.0f);

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
	float dt = engine->GetDeltaTime();
	float cameraSpeed = 1.0f;
	if (inputManager.GetButton(sf::Keyboard::W))
	{
		cameraPos += cameraSpeed * cameraFront * dt;
	}
	if (inputManager.GetButton(sf::Keyboard::S))
	{
		cameraPos -= cameraSpeed * cameraFront * dt;
	}
	if (inputManager.GetButton(sf::Keyboard::A))
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * dt;
	}
	if (inputManager.GetButton(sf::Keyboard::D))
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed* dt;
	}

	auto mousePos = inputManager.GetMousePosition();

	float xoffset = mousePos.x - lastX;
	float yoffset = lastY - mousePos.y; // reversed since y-coordinates go from bottom to top
	lastX = mousePos.x;
	lastY = mousePos.y;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front(0.0f, 0.0f, 0.0f);
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);


	if (fov >= 1.0f && fov <= 45.0f)
		fov -= inputManager.GetMouseWheelDelta() * engine->GetDeltaTime() * fovScroolSpeed;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
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

void HelloTerrainDrawingProgram::UpdateUi() {
	DrawingProgram::UpdateUi();
	ImGui::Separator();
	ImGui::SliderFloat("Terrain Height Mult", &terrainElevationFactor, -10.0f, 10.0f, "height = %.3f");
    ImGui::SliderFloat("Terrain Height Origin", &terrainOriginY, -10.0f, 10.0f, "height = %.3f");
}

int main()
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.bgColor = sf::Color::Black;
	config.windowName = "Hello Terrain";

	engine.AddDrawingProgram(new HelloTerrainDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}
