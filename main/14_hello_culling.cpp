#if WIN32
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include <engine.h>
#include <graphics.h>

#include <Remotery.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "math_utility.h"
#include "imgui.h"
#include "model.h"

const float pixelPerUnit = 100.0f;
#define COUNTER_CLOCK_WISE_CUBE
//#define USE_MODEL

class HelloCullingDrawingProgram : public DrawingProgram
{
public:
	~HelloCullingDrawingProgram() override;
	void Init() override;
	void Draw() override;
	void Destroy() override;
	void UpdateUi() override;
private:
	void ProcessInput();
public:

private:
	
#ifdef USE_MODEL
	Model model;
	Shader modelShaderProgram;
#else
	Shader cubeShaderProgram;
	unsigned int cubeVBO, cubeVAO;
	unsigned int textureWall;
#ifdef COUNTER_CLOCK_WISE_CUBE
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
#else
	float vertices[5 * 36] =
	{
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

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

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	};
#endif
#endif
	std::vector<glm::vec3> cubePositions;

	std::vector<float> cubeInitialAngle;

	int cubeLength = 10'000;
	float near = 0.1f;
	float far = 100.0f;
	bool cpuFilter = false;
	bool enableFaceCulling = true;

};

HelloCullingDrawingProgram::~HelloCullingDrawingProgram()
{
}

void HelloCullingDrawingProgram::Init()
{
	srand(0);
	programName = "HelloCulling";

#ifdef USE_MODEL
    modelShaderProgram.CompileSource(
            "shaders/10_hello_model/model.vert",
            "shaders/10_hello_model/model.frag");
	shaders.push_back(&modelShaderProgram);
	// "data/models/nanosuit/scene.fbx"
	// "data/models/van_gogh_room/Enter a title.obj"
	// 
	model.Init("data/models/nanosuit2/nanosuit.blend");
#else
	shaders.push_back(&cubeShaderProgram);
	cubeShaderProgram.CompileSource("shaders/05_hello_camera/camera.vert", "shaders/05_hello_camera/camera.frag");

	textureWall = gliCreateTexture("data/sprites/wall.dds");
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
#endif
	cubePositions.reserve(cubeLength);
	cubeInitialAngle.reserve(cubeLength);
	for(int i = 0 ; i < 10'000;i++)
	{
		cubePositions.emplace_back(
			RandomRange(-100, 100), 
				RandomRange(-100, 100),
				RandomRange(-100, 0));
		cubeInitialAngle.push_back(M_PI / RandomRange(1,16));
	}

}

void HelloCullingDrawingProgram::Draw()
{
	rmt_ScopedOpenGLSample(DrawCulling);
	rmt_ScopedCPUSample(DrawCullingCPU, 0);
	ProcessInput();

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	glEnable(GL_DEPTH_TEST);
	if (enableFaceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	glm::mat4 view = camera.GetViewMatrix();

#ifdef ORTHOGRAPHIC
	glm::mat4 projection = glm::ortho(
		-(float)config.screenSizeX / pixelPerUnit / 2.0f,
		(float)config.screenSizeX / pixelPerUnit / 2.0f,
		-(float)config.screenSizeY / pixelPerUnit / 2.0f,
		(float)config.screenSizeY / pixelPerUnit / 2.0f,
		0.1f, 100.0f);
#else
	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		(float)config.screenWidth / config.screenHeight, near, far);
#endif

#ifdef USE_MODEL
	modelShaderProgram.Bind();
	modelShaderProgram.SetMat4("view", view);
	modelShaderProgram.SetMat4("projection", projection);
	for (int i = 0; i < cubeLength; i++)
	{
		if (cpuFilter && cubePositions[i].z < -far - 0.5f)
			continue;

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
		modelMatrix = glm::rotate(modelMatrix, (float)engine->GetTimeSinceInit() * glm::radians(50.0f) + cubeInitialAngle[i], glm::vec3(0.5f, 1.0f, 0.0f));

		modelShaderProgram.SetMat4("model", modelMatrix);
		model.Draw(modelShaderProgram);
	}
#else
	cubeShaderProgram.Bind();
	const int viewLoc = glGetUniformLocation(cubeShaderProgram.GetProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	const int projectionLoc = glGetUniformLocation(cubeShaderProgram.GetProgram(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindTexture(GL_TEXTURE_2D, textureWall);
	glBindVertexArray(cubeVAO);
	for (int i = 0; i < cubeLength; i++)
	{
		if(cpuFilter && cubePositions[i].z < -far-0.5f)
			continue;
		
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		model = glm::rotate(model, (float)engine->GetTimeSinceInit() * glm::radians(50.0f) + cubeInitialAngle[i], glm::vec3(0.5f, 1.0f, 0.0f));

		const int modelLoc = glGetUniformLocation(cubeShaderProgram.GetProgram(), "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindVertexArray(0);
#endif
	glDisable(GL_CULL_FACE);
}

void HelloCullingDrawingProgram::Destroy()
{
#ifndef USE_MODEL

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
#endif
}

void HelloCullingDrawingProgram::UpdateUi()
{
	DrawingProgram::UpdateUi();
	ImGui::Separator();
	ImGui::InputFloat("near", &near);
	ImGui::InputFloat("far", &far); 
	ImGui::Checkbox("CPU filter", &cpuFilter);
	ImGui::Checkbox("Enable Face Culling", &enableFaceCulling);
	ImGui::SliderInt("Cube Numbers", &cubeLength, 100,10'000);
}

void HelloCullingDrawingProgram::ProcessInput()
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


int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Culling";
	engine.AddDrawingProgram(new HelloCullingDrawingProgram());
	engine.Init();
	engine.GameLoop();
	// Destroy the main instance of Remotery.
	
	return EXIT_SUCCESS;
}
