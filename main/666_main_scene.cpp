
#include <algorithm>

#include <engine.h>
#include <graphics.h>
#include <geometry.h>
#include <model.h>
#include <light.h>

#include <imgui.h>
#include <Remotery.h>
#include <random>

#define Camera
#ifdef  Camera
class CameraProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
	void ProcessInput();
};

void CameraProgram::Init()
{
	programName = "Camera";
}

void CameraProgram::Draw()
{
	ProcessInput();
}

void CameraProgram::Destroy()
{
}

void CameraProgram::ProcessInput()
{
	Engine * engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();
	auto& inputManager = engine->GetInputManager();

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

	auto mousePos = inputManager.GetMousePosition();

	camera.ProcessMouseMovement(mousePos.x, mousePos.y, true);

	camera.ProcessMouseScroll(inputManager.GetMouseWheelDelta());
}
#endif

#define Terrain
#ifdef Terrain
class TerrainDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
	void UpdateUi() override;
private:
	Shader shaderProgram;

	float lightPos[3] = { 0,0,-9.5 };

	unsigned VBO[2] = {};
	unsigned int VAO = 0;
	unsigned int EBO = 0;

	unsigned terrainTexture = 0;
	unsigned terrainHeightMap = 0;
	unsigned terrainNormalMap = 0;

	float* vertices = nullptr;
	float* texCoords = nullptr;
	unsigned int* indices = nullptr;

	float terrainOriginY = -1.0f;
	float terrainElevationFactor = 5.0f;

	const size_t terrainWidth = 512l;
	const size_t terrainHeight = 512l;
	const float terrainResolution = 0.2f;

	const size_t verticesCount = terrainWidth * terrainHeight;
	const size_t faceCount = 2 * (terrainWidth - 1) * (terrainHeight - 1);

};

void TerrainDrawingProgram::Init()
{
	programName = "Terrain";

	vertices = (float*)calloc(3 * verticesCount, sizeof(float));//vec3, so 3 floats
	texCoords = (float*)calloc(2 * verticesCount, sizeof(float));//vec2, so 2 floats

	for (size_t i = 0l; i < verticesCount; i++)
	{
		vertices[3 * i] = -(float)terrainWidth * terrainResolution / 2.0f + (float)(i % terrainWidth) * terrainResolution;//x
		vertices[3 * i + 1] = 0.0f;//y
		vertices[3 * i + 2] = -(float)terrainHeight * terrainResolution / 2.0f + (float)(i / terrainWidth) * terrainResolution;//z
	}
	for (size_t i = 0l; i < verticesCount; i++)
	{
		const float width = terrainWidth;
		const float height = terrainHeight;
		texCoords[2 * i] = (float)((i % terrainWidth) + 1) / (width + 1);
		texCoords[2 * i + 1] = (float)((i / terrainWidth) + 1) / (height + 1);
	}

	indices = (unsigned *)calloc(3l * faceCount, sizeof(unsigned));
	size_t quad = 0;
	for (size_t y = 0; y < terrainHeight - 1; y++)
	{
		for (size_t x = 0; x < terrainWidth - 1; x++)
		{
			const unsigned origin = x + y * terrainWidth;
			const unsigned originBottom = origin + terrainWidth;

			//face1
			indices[6 * quad] = origin;
			indices[6 * quad + 1] = origin + 1;
			indices[6 * quad + 2] = originBottom;

			//face2
			indices[6 * quad + 3] = origin + 1;
			indices[6 * quad + 4] = originBottom + 1;
			indices[6 * quad + 5] = originBottom;

			quad++;
		}
	}

	shaderProgram.CompileSource("shaders/666_main_scene/terrain.vert", "shaders/666_main_scene/terrain.frag");
	shaders.push_back(&shaderProgram);

	terrainHeightMap = stbCreateTexture("data/terrain/plains/HeightMap.png", true, false);
	terrainTexture = stbCreateTexture("data/terrain/plains/texture.png", true, false);
	terrainNormalMap = stbCreateTexture("data/terrain/plains/NormalMap.png", true, false);

	glGenBuffers(2, &VBO[0]);
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO); //like: new VAO()
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);//Now use our VAO
	//bind vertices data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
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

void TerrainDrawingProgram::Draw()
{
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	rmt_BeginOpenGLSample(HelloTerrainDraw);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / config.screenHeight, 0.1f, 10000.0f);

	shaderProgram.Bind();
	shaderProgram.SetVec3("lightPos", lightPos);
	shaderProgram.SetVec3("viewPos", camera.Position);

	shaderProgram.SetMat4("view", view);
	shaderProgram.SetMat4("projection", projection);
	shaderProgram.SetMat4("model", model);
	shaderProgram.SetFloat("heightResolution", terrainElevationFactor);
	shaderProgram.SetFloat("heightOrigin", terrainOriginY);

	shaderProgram.SetInt("heightMap", 0);
	shaderProgram.SetInt("diffuseMap", 1);
	shaderProgram.SetInt("normalMap", 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainHeightMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrainTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, terrainNormalMap);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
	rmt_EndOpenGLSample();
}

void TerrainDrawingProgram::Destroy()
{
	free(vertices);
	free(texCoords);
	free(indices);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(1, &EBO);
}

void TerrainDrawingProgram::UpdateUi()
{
	DrawingProgram::UpdateUi();
	ImGui::Separator();
	ImGui::SliderFloat("Terrain Height Mult", &terrainElevationFactor, -10.0f, 10.0f, "height = %.3f");
	ImGui::SliderFloat("Terrain Height Origin", &terrainOriginY, -10.0f, 10.0f, "height = %.3f");
}
#endif

#define Models
#ifdef Models
const int numTrees = 50;
const int numBushes = 20;
const int numFlowers = 150;

class ModelDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;

private:
	void InitModels();
	void DrawModels();

	Shader modelShaderProgram;

	Model treeModel;
	Model bushModel;
	Model flowerModel;

	GLfloat treePosition[3 * numTrees] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		2.0f, 2.0f, 2.0f,
		3.0f, 3.0f, 3.0f,
		4.0f, 4.0f, 4.0f,
		5.0f, 5.0f, 5.0f,
		6.0f, 6.0f, 6.0f,
		7.0f, 7.0f, 7.0f,
		8.0f, 8.0f, 8.0f,
		9.0f, 9.0f, 9.0f,
		10.0f, 10.0f, 10.0f,
		11.0f, 11.0f, 11.0f,
		12.0f, 12.0f, 12.0f,
		13.0f, 13.0f, 13.0f,
		14.0f, 14.0f, 14.0f,
		15.0f, 15.0f, 15.0f,
		16.0f, 16.0f, 16.0f,
		17.0f, 17.0f, 17.0f,
		18.0f, 18.0f, 18.0f,
		19.0f, 19.0f, 19.0f,
		20.0f, 20.0f, 20.0f,
		21.0f, 21.0f, 21.0f,
		22.0f, 22.0f, 22.0f,
		23.0f, 23.0f, 23.0f,
		24.0f, 24.0f, 24.0f,
		25.0f, 25.0f, 25.0f,
		26.0f, 26.0f, 26.0f,
		27.0f, 27.0f, 27.0f,
		28.0f, 28.0f, 28.0f,
		29.0f, 29.0f, 29.0f,
		30.0f, 30.0f, 30.0f,
		31.0f, 31.0f, 31.0f,
		32.0f, 32.0f, 32.0f,
		33.0f, 33.0f, 33.0f,
		34.0f, 34.0f, 34.0f,
		35.0f, 35.0f, 35.0f,
		36.0f, 36.0f, 36.0f,
		37.0f, 37.0f, 37.0f,
		38.0f, 38.0f, 38.0f,
		39.0f, 39.0f, 39.0f,
		40.0f, 40.0f, 40.0f,
		41.0f, 41.0f, 41.0f,
		42.0f, 42.0f, 42.0f,
		43.0f, 43.0f, 43.0f,
		44.0f, 44.0f, 44.0f,
		45.0f, 45.0f, 45.0f,
		46.0f, 46.0f, 46.0f,
		47.0f, 47.0f, 47.0f,
		48.0f, 48.0f, 48.0f,
		49.0f, 49.0f, 49.0f
	};

	GLfloat bushPosition[3 * numBushes] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		2.0f, 2.0f, 2.0f,
		3.0f, 3.0f, 3.0f,
		4.0f, 4.0f, 4.0f,
		5.0f, 5.0f, 5.0f,
		6.0f, 6.0f, 6.0f,
		7.0f, 7.0f, 7.0f,
		8.0f, 8.0f, 8.0f,
		9.0f, 9.0f, 9.0f,
		10.0f, 10.0f, 10.0f,
		11.0f, 11.0f, 11.0f,
		12.0f, 12.0f, 12.0f,
		13.0f, 13.0f, 13.0f,
		14.0f, 14.0f, 14.0f,
		15.0f, 15.0f, 15.0f,
		16.0f, 16.0f, 16.0f,
		17.0f, 17.0f, 17.0f,
		18.0f, 18.0f, 18.0f,
		19.0f, 19.0f, 19.0f
	};

	GLfloat flowerPosition[3 * numFlowers] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		2.0f, 2.0f, 2.0f,
		3.0f, 3.0f, 3.0f,
		4.0f, 4.0f, 4.0f,
		5.0f, 5.0f, 5.0f,
		6.0f, 6.0f, 6.0f,
		7.0f, 7.0f, 7.0f,
		8.0f, 8.0f, 8.0f,
		9.0f, 9.0f, 9.0f,
		10.0f, 10.0f, 10.0f,
		11.0f, 11.0f, 11.0f,
		12.0f, 12.0f, 12.0f,
		13.0f, 13.0f, 13.0f,
		14.0f, 14.0f, 14.0f,
		15.0f, 15.0f, 15.0f,
		16.0f, 16.0f, 16.0f,
		17.0f, 17.0f, 17.0f,
		18.0f, 18.0f, 18.0f,
		19.0f, 19.0f, 19.0f,
		20.0f, 20.0f, 20.0f,
		21.0f, 21.0f, 21.0f,
		22.0f, 22.0f, 22.0f,
		23.0f, 23.0f, 23.0f,
		24.0f, 24.0f, 24.0f,
		25.0f, 25.0f, 25.0f,
		26.0f, 26.0f, 26.0f,
		27.0f, 27.0f, 27.0f,
		28.0f, 28.0f, 28.0f,
		29.0f, 29.0f, 29.0f,
		30.0f, 30.0f, 30.0f,
		31.0f, 31.0f, 31.0f,
		32.0f, 32.0f, 32.0f,
		33.0f, 33.0f, 33.0f,
		34.0f, 34.0f, 34.0f,
		35.0f, 35.0f, 35.0f,
		36.0f, 36.0f, 36.0f,
		37.0f, 37.0f, 37.0f,
		38.0f, 38.0f, 38.0f,
		39.0f, 39.0f, 39.0f,
		40.0f, 40.0f, 40.0f,
		41.0f, 41.0f, 41.0f,
		42.0f, 42.0f, 42.0f,
		43.0f, 43.0f, 43.0f,
		44.0f, 44.0f, 44.0f,
		45.0f, 45.0f, 45.0f,
		46.0f, 46.0f, 46.0f,
		47.0f, 47.0f, 47.0f,
		48.0f, 48.0f, 48.0f,
		49.0f, 49.0f, 49.0f,
		50.0f, 0.0f, 50.0f,
		51.0f, 1.0f, 51.0f,
		52.0f, 2.0f, 52.0f,
		53.0f, 3.0f, 53.0f,
		54.0f, 4.0f, 54.0f,
		55.0f, 5.0f, 55.0f,
		56.0f, 6.0f, 56.0f,
		57.0f, 7.0f, 57.0f,
		58.0f, 8.0f, 58.0f,
		59.0f, 9.0f, 59.0f,
		60.0f, 0.0f, 60.0f,
		61.0f, 1.0f, 61.0f,
		62.0f, 2.0f, 62.0f,
		63.0f, 3.0f, 63.0f,
		64.0f, 4.0f, 64.0f,
		65.0f, 5.0f, 65.0f,
		66.0f, 6.0f, 66.0f,
		67.0f, 7.0f, 67.0f,
		68.0f, 8.0f, 68.0f,
		69.0f, 9.0f, 69.0f,
		70.0f, 0.0f, 70.0f,
		71.0f, 1.0f, 71.0f,
		72.0f, 2.0f, 72.0f,
		73.0f, 3.0f, 73.0f,
		74.0f, 4.0f, 74.0f,
		75.0f, 5.0f, 75.0f,
		76.0f, 6.0f, 76.0f,
		77.0f, 7.0f, 77.0f,
		78.0f, 8.0f, 78.0f,
		79.0f, 9.0f, 79.0f,
		80.0f, 0.0f, 80.0f,
		81.0f, 1.0f, 81.0f,
		82.0f, 2.0f, 82.0f,
		83.0f, 3.0f, 83.0f,
		84.0f, 4.0f, 84.0f,
		85.0f, 5.0f, 85.0f,
		86.0f, 6.0f, 86.0f,
		87.0f, 7.0f, 87.0f,
		88.0f, 8.0f, 88.0f,
		89.0f, 9.0f, 89.0f,
		90.0f, 0.0f, 90.0f,
		91.0f, 1.0f, 91.0f,
		92.0f, 2.0f, 92.0f,
		93.0f, 3.0f, 93.0f,
		94.0f, 4.0f, 94.0f,
		95.0f, 5.0f, 95.0f,
		96.0f, 6.0f, 96.0f,
		97.0f, 7.0f, 97.0f,
		98.0f, 8.0f, 98.0f,
		99.0f, 9.0f, 99.0f,
		100.0f, 0.0f, 0.0f,
		101.0f, 1.0f, 101.0f,
		102.0f, 2.0f, 102.0f,
		103.0f, 3.0f, 103.0f,
		104.0f, 4.0f, 104.0f,
		105.0f, 5.0f, 105.0f,
		106.0f, 6.0f, 106.0f,
		107.0f, 7.0f, 107.0f,
		108.0f, 8.0f, 108.0f,
		109.0f, 9.0f, 109.0f,
		110.0f, 10.0f, 110.0f,
		111.0f, 11.0f, 111.0f,
		112.0f, 12.0f, 112.0f,
		113.0f, 13.0f, 113.0f,
		114.0f, 14.0f, 114.0f,
		115.0f, 15.0f, 115.0f,
		116.0f, 16.0f, 116.0f,
		117.0f, 17.0f, 117.0f,
		118.0f, 18.0f, 118.0f,
		119.0f, 19.0f, 119.0f,
		120.0f, 20.0f, 120.0f,
		121.0f, 21.0f, 121.0f,
		122.0f, 22.0f, 122.0f,
		123.0f, 23.0f, 123.0f,
		124.0f, 24.0f, 124.0f,
		125.0f, 25.0f, 125.0f,
		126.0f, 26.0f, 126.0f,
		127.0f, 27.0f, 127.0f,
		128.0f, 28.0f, 128.0f,
		129.0f, 29.0f, 129.0f,
		130.0f, 30.0f, 130.0f,
		131.0f, 31.0f, 131.0f,
		132.0f, 32.0f, 132.0f,
		133.0f, 33.0f, 133.0f,
		134.0f, 34.0f, 134.0f,
		135.0f, 35.0f, 135.0f,
		136.0f, 36.0f, 136.0f,
		137.0f, 37.0f, 137.0f,
		138.0f, 38.0f, 138.0f,
		139.0f, 39.0f, 139.0f,
		140.0f, 40.0f, 140.0f,
		141.0f, 41.0f, 141.0f,
		142.0f, 42.0f, 142.0f,
		143.0f, 43.0f, 143.0f,
		144.0f, 44.0f, 144.0f,
		145.0f, 45.0f, 145.0f,
		146.0f, 46.0f, 146.0f,
		147.0f, 47.0f, 147.0f,
		148.0f, 48.0f, 148.0f,
		149.0f, 49.0f, 149.0f
	};

	GLuint treePositionBuffer;
	GLuint bushPositionBuffer;
	GLuint flowerPositionBuffer;

	glm::mat4* modelMatrices;

	//Ambient and SSAO
	Shader hdrShader;
	Plane hdrPlane;
	unsigned hdrFBO = 0;
	unsigned rboDepth = 0;
	unsigned hdrColorBuffer[2];

	Shader lightShader;
	Shader lightingPassShader;
	int lightNmb = 1;
	DirectionLight directionLight;
	float exposure = 0.1f;
	float lightIntensity = 1.0f;

	Shader modelDeferredShader;
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gColorSpec, gAlbedoSpec, gSSAOAlbedo;
	unsigned int gRBO;

	unsigned ssaoFBO;
	unsigned ssaoColorBuffer;
	Shader ssaoPassShader;

	unsigned ssaoBlurFBO;
	unsigned ssaoColorBufferBlur;
	Shader ssaoBlurPassShader;
	std::vector<glm::vec3> ssaoKernel;
	unsigned noiseTexture;

	float ssaoRadius = 0.5f;
	int kernelSize = 64;
};

void ModelDrawingProgram::Init()
{
	programName = "Models";
	auto* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	InitModels();

	/******************************************************************************/
	/***						SSAO and Ambiant occlusion						***/
	/******************************************************************************/
	modelDeferredShader.CompileSource(
		"shaders/666_main_scene/ssao_basic.vert",
		"shaders/666_main_scene/ssao_basic.frag");
	lightingPassShader.CompileSource(
		"shaders/666_main_scene/lighting_pass.vert",
		"shaders/666_main_scene/lighting_pass.frag");

	//deferred Framebuffer	
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, config.screenWidth, config.screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, config.screenWidth, config.screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		config.screenWidth, config.screenHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - ssao color buffer
	glGenTextures(1, &gSSAOAlbedo);
	glBindTexture(GL_TEXTURE_2D, gSSAOAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		config.screenWidth, config.screenHeight, 0,
		GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSSAOAlbedo, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int gAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, gAttachments);
	glGenRenderbuffers(1, &gRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32,
		config.screenWidth,
		config.screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
		gRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Ambient occlusion
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, config.screenWidth, config.screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ssaoPassShader.CompileSource(
		"shaders/666_main_scene/ssao_pass.vert",
		"shaders/666_main_scene/ssao_pass.frag");



	// generate noise texture
	// ----------------------
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Ambient occlusion blur
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, config.screenWidth, config.screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ssaoBlurPassShader.CompileSource(
		"shaders/666_main_scene/ssao_blur.vert",
		"shaders/666_main_scene/ssao_blur.frag");
	
}

void ModelDrawingProgram::InitModels()
{
	
	modelShaderProgram.CompileSource(
		"shaders/666_main_scene/model_instanced.vert",
		"shaders/666_main_scene/model_instanced.frag");
	shaders.push_back(&modelShaderProgram);
	

	treeModel.Init("data/models/voxel_tree/Tree.obj", true);
	bushModel.Init("data/models/voxel_bush/Bush.obj", true);
	flowerModel.Init("data/models/voxel_flower/Flower.obj", true);

	/******************************************************************************/
	/***								Loading trees							***/
	/******************************************************************************/
	modelMatrices = new glm::mat4[numTrees];
	for (unsigned int i = 0; i < numTrees; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::translate(model, glm::vec3(treePosition[3 * i + 0], treePosition[3 * i + 1], treePosition[3 * i + 2]));

		modelMatrices[i] = model;
	}

	// configure instanced array
	// -------------------------
	glGenBuffers(1, &treePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, treePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, numTrees * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < treeModel.meshes.size(); i++)
	{
		unsigned int VAO = treeModel.meshes[i].GetVAO();
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}

	/******************************************************************************/
	/***								Loading bushes							***/
	/******************************************************************************/
	modelMatrices = new glm::mat4[numBushes];
	for (unsigned int i = 0; i < numBushes; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::translate(model, glm::vec3(bushPosition[3 * i + 0], bushPosition[3 * i + 1], bushPosition[3 * i + 2]));

		modelMatrices[i] = model;
	}

	// configure instanced array
	// -------------------------
	glGenBuffers(1, &bushPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bushPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, numBushes * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < bushModel.meshes.size(); i++)
	{
		unsigned int VAO = bushModel.meshes[i].GetVAO();
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 0);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}

	/******************************************************************************/
	/***								Loading flowers							***/
	/******************************************************************************/
	modelMatrices = new glm::mat4[numFlowers];
	for (unsigned int i = 0; i < numFlowers; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::translate(model, glm::vec3(flowerPosition[3 * i + 0], flowerPosition[3 * i + 1], flowerPosition[3 * i + 2]));

		modelMatrices[i] = model;
	}

	// configure instanced array
	// -------------------------
	glGenBuffers(1, &flowerPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, flowerPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, numFlowers * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < flowerModel.meshes.size(); i++)
	{
		unsigned int VAO = flowerModel.meshes[i].GetVAO();
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}
}

void ModelDrawingProgram::Draw()
{
	rmt_ScopedCPUSample(SSAO_CPU, 0);
	rmt_ScopedOpenGLSample(SSAO_GPU);
	rmt_BeginOpenGLSample(G_Buffer);

	auto* engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();
	auto& config = engine->GetConfiguration();
	glm::mat4 projection = glm::perspective(camera.Zoom,(float)config.screenWidth / config.screenHeight,0.1f, 10000.0f);
	
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	DrawModels();
	rmt_EndOpenGLSample(); //GBUFFER

	//Ambient occlusion pass
	
	rmt_BeginOpenGLSample(AO_Pass);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	{
		glClear(GL_COLOR_BUFFER_BIT);
		ssaoPassShader.Bind();

		// generate sample kernel
		// ----------------------
		ssaoKernel.clear();
		std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
		std::default_random_engine generator;

		for (unsigned int i = 0; i < kernelSize; ++i)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / kernelSize;

			// scale samples s.t. they're more aligned to center of kernel
			scale = 0.1f + scale * scale * (1.0f - 0.1f); //a + f * (b - a);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}
		// Send kernel + rotation 
		for (unsigned int i = 0; i < kernelSize; ++i)
			ssaoPassShader.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);

		ssaoPassShader.SetInt("kernelSize", kernelSize);
		ssaoPassShader.SetFloat("radius", ssaoRadius);
		ssaoPassShader.SetMat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		ssaoPassShader.SetInt("gPosition", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);

		ssaoPassShader.SetInt("gNormal", 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);

		ssaoPassShader.SetInt("texNoise", 2);
		ssaoPassShader.SetVec2("noiseScale", glm::vec2(config.screenWidth / 4.0f, config.screenHeight / 4.0f));
		hdrPlane.Draw();
	}
	rmt_EndOpenGLSample();

	rmt_BeginOpenGLSample(SSAO_Blur);
	{
		// Blur SSAO texture to remove noise
		// ------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		ssaoBlurPassShader.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		hdrPlane.Draw();
	}
	rmt_EndOpenGLSample();

	rmt_BeginOpenGLSample(LightPass);
	//Light pass
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//copy depth buffer into hdrFBO
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0, config.screenWidth, config.screenHeight, 0, 0, config.screenWidth, config.screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
		lightingPassShader.Bind();

		directionLight.intensity = lightIntensity;
		directionLight.Bind(lightingPassShader, 0);
		//light pass
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
		lightingPassShader.SetInt("gPosition", 0);
		lightingPassShader.SetInt("gNormal", 1);
		lightingPassShader.SetInt("gAlbedoSpec", 2);
		lightingPassShader.SetInt("gSsaoAlbedo", 3);
		lightingPassShader.SetInt("pointLightsNmb", lightNmb);
		lightingPassShader.SetVec3("viewPos", camera.Position);
		//render light in forward rendering
		hdrPlane.Draw();
		glEnable(GL_DEPTH_TEST);
	}
	lightShader.Bind();
	rmt_EndOpenGLSample();
}

void ModelDrawingProgram::DrawModels()
{
	rmt_BeginOpenGLSample(ModelDraw);
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 10000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	//glm::mat4 VP = projection * view;

	rmt_BeginOpenGLSample(TreeDraw);
	// Draw the trees
	modelDeferredShader.Bind();
	modelDeferredShader.SetMat4("view", view);
	modelDeferredShader.SetMat4("projection", projection);
	modelDeferredShader.SetMat4("VP", projection * view);
	this->treeModel.Draw(modelDeferredShader, numTrees);
	rmt_EndOpenGLSample(); // TreeDraw

	rmt_BeginOpenGLSample(BushDraw);
	// Draw the bushes
	//bushShaderProgram.Bind();
	//bushShaderProgram.SetMat4("VP", VP);
	this->bushModel.Draw(modelDeferredShader, numBushes);
	rmt_EndOpenGLSample(); // BushDraw

	rmt_BeginOpenGLSample(FlowerDraw);
	// Draw the flowers
	//flowerShaderProgram.Bind();
	//flowerShaderProgram.SetMat4("VP", VP);
	this->flowerModel.Draw(modelDeferredShader, numFlowers);
	rmt_EndOpenGLSample(); // FlowerDraw

	glDisable(GL_CULL_FACE);
	rmt_EndOpenGLSample();
}

void ModelDrawingProgram::Destroy()
{
	//Delete buffers
	glDeleteBuffers(1, &treePositionBuffer);
	glDeleteBuffers(1, &bushPositionBuffer);
	glDeleteBuffers(1, &flowerPositionBuffer);

	//TODO : Implement the destroy of models
	//treeModel.Destroy();
	//bushModel.Destroy();
	//flowerModel.Destroy();
}
#endif

#define Firefly
#ifdef Firefly
// CPU representation of a particle
struct FireflyParticle {
	glm::vec3 startPos, destPos, diffPos, position, color;
	float timeSinceBegin, timeToEnd, cameraDistance; // *Squared* distance to the camera. if dead : -1.0f
	float scale, glowScale, glowSpeed;
	bool operator<(const FireflyParticle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameraDistance > that.cameraDistance;
	}
};

const int MaxParticles = 8000;

class FireflyDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;

	void UpdateUi() override;

	int ProcessParticles(float dt);
	void SortParticles();

private:
	Shader fireflyShaderProgram;
	Shader blurShader;
	Shader hdrShader;

	//Consts (Editable with Imgui)
	int NumFireflies = MaxParticles;
	GLfloat botRightLimit[3] = { -10.0f, -10.0f, -10.0f };
	int range[3] = { 200, 200, 200 };

	Plane hdrPlane;
	GLuint hdrFBO = 0;
	GLuint rboDepth = 0;
	GLuint hdrColorBuffer[2];
	GLuint hdrDepthBuffer = 0;

	GLuint pingpongFBO[2];
	GLuint pingpongBuffer[2];

	GLuint fireflyTexture;

	GLuint VAO;
	GLuint VBO[2];
	GLuint EBO;
	//Billboard
	GLfloat vertices[12] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left
	};
	GLfloat texCoords[8] = {
		1.0f, 0.0f,	  // top right
		1.0f, 1.0f,   // bottom right
		0.0f, 1.0f,   // bottom left
		0.0f, 0.0f,   // top left
	};
	unsigned int indices[6] = {
		// note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	std::vector<FireflyParticle> ParticlesContainer;
	int LastUsedParticle = 0; //Used to know how much fireflies are empty
	GLfloat exposure = 1.6f;

	GLfloat particlesPosition[3 * MaxParticles];
	GLfloat particlesColor[3 * MaxParticles];

	// 1 for scale (for the 3 axis)
	// 1 for glowing effect
	// 1 for glowing speed
	GLfloat particlesScale[3 * MaxParticles];

	GLuint particlesPositionBuffer;
	GLuint particlesColorBuffer;
	GLuint particlesScaleBuffer;
};

void FireflyDrawingProgram::Init()
{
	programName = "Firefly";

	auto* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();

	ParticlesContainer.resize(NumFireflies);
	//Init particles
	for (int i = 0; i < NumFireflies; i++) {
		ParticlesContainer[i].destPos = glm::vec3(
			rand() % range[0] + botRightLimit[0],
			rand() % range[1] + botRightLimit[1],
			rand() % range[2] + botRightLimit[2]
		);

		ParticlesContainer[i].timeSinceBegin = -1.0f;
		ParticlesContainer[i].timeToEnd = -1.0f;
		ParticlesContainer[i].cameraDistance = -1.0f;

		LastUsedParticle = i;

		// Buffering init values for each particle
		//float exposure = rand() % 64;
		ParticlesContainer[i].color.r = (rand() % 32) / 256.0f;
		ParticlesContainer[i].color.g = (rand() % 78) / 256.0f;
		ParticlesContainer[i].color.b = (rand() % 32) / 256.0f;

		ParticlesContainer[i].scale = (rand() % 4) / 5.0f + 0.8f;
		ParticlesContainer[i].glowScale = std::max((rand() % 10) / 10.0f - 0.3f, 0.2f) / 3.0f;
		ParticlesContainer[i].glowSpeed = (rand() % 6 + 4.0f) / 2.0f;
	}

	fireflyShaderProgram.CompileSource("shaders/666_main_scene/firefly.vert", "shaders/666_main_scene/firefly.frag"); // quad
	shaders.push_back(&fireflyShaderProgram);

	hdrShader.CompileSource("shaders/666_main_scene/hdr.vert", "shaders/666_main_scene/hdr.frag");
	shaders.push_back(&hdrShader);

	blurShader.CompileSource("shaders/666_main_scene/hdr.vert", "shaders/666_main_scene/blur.frag");

	fireflyTexture = stbCreateTexture("data/sprites/firefly6.png", true, true, true);

	//////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////			BINDING HDR 		//////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////
	hdrPlane.Init();
	glGenFramebuffers(1, &hdrFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glGenTextures(2, hdrColorBuffer);
	glGenTextures(1, &hdrDepthBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, hdrColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
			config.screenWidth, config.screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D, hdrColorBuffer[i], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, hdrDepthBuffer, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32,
		config.screenWidth,
		config.screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
		rboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth, config.screenHeight, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////			BINDING VAO			//////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////

	glGenBuffers(2, &VBO[0]);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &particlesPositionBuffer);
	glGenBuffers(1, &particlesColorBuffer);
	glGenBuffers(1, &particlesScaleBuffer);

	glGenVertexArrays(1, &VAO); //like: new VAO()
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);//Now use our VAO
	//bind vertices data
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	//bind texture coords data
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

	//bind vertices index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//bind particle Color
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particlesColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(GLfloat), particlesColor, GL_STREAM_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0); // Size of Color

	//bind particle Scale
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, particlesScaleBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(GLfloat), particlesScale, GL_STREAM_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0); // Size of Scale

	//bind particle position data
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, particlesPositionBuffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(float), NULL, GL_STREAM_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Size of Position

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	//unbind Vertex Array
	glBindVertexArray(0);
}

void FireflyDrawingProgram::Draw()
{
	Engine* engine = Engine::GetPtr();
	Configuration& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();
	float dt = engine->GetDeltaTime();

	//Get matrices
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 10000.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Bind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	//Clear color OF THE BUFFER
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	fireflyShaderProgram.Bind();

	//bind texture
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fireflyTexture);

	//Process particles
	const int ParticlesCount = ProcessParticles(dt);

	// Pass billboard reuqired informations to the shader
	fireflyShaderProgram.SetVec3("CameraRight", camera.Right);
	fireflyShaderProgram.SetVec3("CameraUp", camera.Up);
	fireflyShaderProgram.SetMat4("VP", projection * viewMatrix);
	fireflyShaderProgram.SetFloat("time", engine->GetTimeSinceInit());

	// Update the buffer with all the positions
	glBindBuffer(GL_ARRAY_BUFFER, particlesPositionBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * 3 * sizeof(float), &particlesPosition);

	// Update the buffer with all the Colors
	glBindBuffer(GL_ARRAY_BUFFER, particlesColorBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * 3 * sizeof(float), &particlesColor);

	// Update the buffer with all the Scale
	glBindBuffer(GL_ARRAY_BUFFER, particlesScaleBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * 3 * sizeof(float), &particlesScale);

	// Draw the particles
	//glDepthFunc(GL_ALWAYS);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ParticlesCount);
	glDepthMask(GL_FALSE);
	//glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE); // Don't write to depth buffer

	//glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ParticlesCount);
	// Post processing pass
	bool horizontal = true, first_iteration = true;
	int amount = 10;
	blurShader.Bind();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		blurShader.SetInt("horizontal", horizontal);
		blurShader.SetInt("image", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? hdrColorBuffer[1] : pingpongBuffer[!horizontal]
		);
		hdrPlane.Draw();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	//Show hdr quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	hdrShader.Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrColorBuffer[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
	hdrShader.SetInt("hdrBuffer", 0);
	hdrShader.SetInt("bloomBlur", 1);
	hdrShader.SetFloat("exposure", exposure);

	hdrPlane.Draw();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void FireflyDrawingProgram::Destroy()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	// Delete texture
	glDeleteTextures(1, &fireflyTexture);

	//Delete vertex Arrays
	glDeleteVertexArrays(1, &VAO);

	// Delete Buffers
	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(1, &EBO);

	glDeleteBuffers(1, &particlesPositionBuffer);
	glDeleteBuffers(1, &particlesColorBuffer);
	glDeleteBuffers(1, &particlesScaleBuffer);

	//Delete Render buffers
	glDeleteRenderbuffers(1, &rboDepth);

	//Delete frame buffers
	glDeleteFramebuffers(1, &hdrFBO);
	glDeleteFramebuffers(2, pingpongFBO);
}

void FireflyDrawingProgram::UpdateUi()
{
	ImGui::Separator();
	ImGui::InputInt3("Limite top left", range);
	ImGui::InputFloat3("Limite bottom right", botRightLimit);
	ImGui::SliderInt("Num fireflies", &NumFireflies, 1, MaxParticles);
	ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
	ImGui::Separator();
}

int FireflyDrawingProgram::ProcessParticles(float dt)
{
	auto* engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();

	// Simulate all particles
	int ParticlesCount = 0;

	//For each particles
	for (int i = 0; i < NumFireflies; i++)
	{
		FireflyParticle& p = ParticlesContainer[i];

		// Decrease traveltime
		p.timeSinceBegin -= dt;

		// if firefly arrived to its destination
		if (p.timeSinceBegin < 0.0f)
		{
			//Find a new location to go !
			p.startPos = p.destPos;
			p.destPos = glm::vec3(
				rand() % range[0] + botRightLimit[0],
				rand() % range[1] + botRightLimit[1],
				rand() % range[2] + botRightLimit[2]
			);
			p.diffPos = p.destPos - p.startPos;

			//Calculate its speed to destination
			p.timeToEnd = glm::length(p.diffPos) / (rand() % 4 * 1.0f + 1);
			p.timeSinceBegin = p.timeToEnd;
		}

		// Parametric blend, ease in and out. Results in this : https://www.wolframalpha.com/input/?i=t%5E2(3-2t)
		// Go from 0 to 1. (due to 1 - {...})
		float t = 1 - p.timeSinceBegin / p.timeToEnd;
		float change = pow(t, 2) / (2.0f * (pow(t, 2) - t) + 1.0f);

		p.position = (p.startPos + p.diffPos * change) - camera.Position * 5.0f;
		// Culling of the particles (if behind of the camera, then we don't render them.
		p.cameraDistance = glm::dot(p.position, camera.Front) - glm::dot(camera.Position, camera.Front);
		if (p.cameraDistance > 0.0f)
			ParticlesCount++;
	}

	// Order the particles
	SortParticles();

	//Affect them to the buffer
	for (int i = 0; i < ParticlesCount; i++)
	{
		FireflyParticle& p = ParticlesContainer[i];

		// Calculate position and fill GPU budder
		particlesPosition[3 * i + 0] = p.position.x;
		particlesPosition[3 * i + 1] = p.position.y;
		particlesPosition[3 * i + 2] = p.position.z;

		particlesColor[3 * i + 0] = p.color.r;
		particlesColor[3 * i + 1] = p.color.g;
		particlesColor[3 * i + 2] = p.color.b;

		particlesScale[3 * i + 0] = p.scale;
		particlesScale[3 * i + 1] = p.glowScale;
		particlesScale[3 * i + 2] = p.glowSpeed;
	}

	return ParticlesCount;
}

void FireflyDrawingProgram::SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles - 1]);
}
#endif

#define Skybox
#ifdef Skybox
class SkyboxDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;

private:
	GLuint cubemapTexture;
	Shader cubemapShader;
	GLuint cubeMapVAO;
	GLuint cubeMapVBO;

	float vertices[5 * 36] =
	{
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	};
};

void SkyboxDrawingProgram::Init()
{
	programName = "Skybox";

	cubemapShader.CompileSource(
		"shaders/666_main_scene/skybox.vert",
		"shaders/666_main_scene/skybox.frag"
	);
	shaders.push_back(&cubemapShader);
	/*std::vector<std::string> faces =
	{
		"data/skybox/nebula/purplenebula_lf.tga",
		"data/skybox/nebula/purplenebula_rt.tga",
		"data/skybox/nebula/purplenebula_up.tga",
		"data/skybox/nebula/purplenebula_dn.tga",
		"data/skybox/nebula/purplenebula_ft.tga",
		"data/skybox/nebula/purplenebula_bk.tga"
	};*/
	/*std::vector<std::string> faces =
	{
		"data/skybox/emerald/emeraldfog_lf.tga",
		"data/skybox/emerald/emeraldfog_rt.tga",
		"data/skybox/emerald/emeraldfog_up.tga",
		"data/skybox/emerald/emeraldfog_dn.tga",
		"data/skybox/emerald/emeraldfog_ft.tga",
		"data/skybox/emerald/emeraldfog_bk.tga"
	};*/
	std::vector<std::string> faces =
	{
		"data/skybox/fluffballday/FluffballDayLeft.hdr",
		"data/skybox/fluffballday/FluffballDayRight.hdr",
		"data/skybox/fluffballday/FluffballDayTop.hdr",
		"data/skybox/fluffballday/FluffballDayBottom.hdr",
		"data/skybox/fluffballday/FluffballDayFront.hdr",
		"data/skybox/fluffballday/FluffballDayBack.hdr"
	};
	cubemapTexture = LoadCubemap(faces);

	glGenVertexArrays(1, &cubeMapVAO);
	glGenBuffers(1, &cubeMapVBO);

	glBindVertexArray(cubeMapVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeMapVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void SkyboxDrawingProgram::Draw()
{
	auto* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	const glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 10000.0f);

	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);
	const glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	cubemapShader.Bind();
	cubemapShader.SetMat4("projection", projection);
	cubemapShader.SetMat4("view", skyboxView);
	glBindVertexArray(cubeMapVAO);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void SkyboxDrawingProgram::Destroy()
{
	glDeleteVertexArrays(1, &cubeMapVAO);
	glDeleteBuffers(1, &cubeMapVBO);
}
#endif

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1920;
	config.screenHeight = 1024;
	config.windowName = "Main Scene";
	config.bgColor = { 1,1,1,1 };

#ifdef Camera
	engine.AddDrawingProgram(new CameraProgram());
#endif

#ifdef Skybox
	engine.AddDrawingProgram(new SkyboxDrawingProgram());
#endif

#ifdef Terrain
	engine.AddDrawingProgram(new TerrainDrawingProgram());
#endif

#ifdef Models
	engine.AddDrawingProgram(new ModelDrawingProgram());
#endif

#ifdef Firefly
	engine.AddDrawingProgram(new FireflyDrawingProgram());
#endif

	engine.Init();

	engine.GameLoop();
	return EXIT_SUCCESS;
}