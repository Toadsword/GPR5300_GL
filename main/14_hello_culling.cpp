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
#define CAMERA_CONTROLS
#define COUNTER_CLOCK_WISE_CUBE
#define USE_MODEL



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
#ifdef CAMERA_CONTROLS
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	float fov = 45.0f;
	float fovScroolSpeed = 100.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
#endif
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
	
#ifdef CAMERA_CONTROLS
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	lastX = config.screenWidth / 2.0f;
	lastY = config.screenHeight / 2.0f;
#endif
#ifdef USE_MODEL
    modelShaderProgram.CompileSource(
            "shaders/10_hello_model/model.vert",
            "shaders/10_hello_model/model.frag");
	shaders.push_back(&modelShaderProgram);
	// "data/models/nanosuit/scene.fbx"
	// "data/models/van_gogh_room/Enter a title.obj"
	// 
	model.Init("data/models/nanosuit2/nanosuit.obj");
#else
	shaders.push_back(&cubeShaderProgram);
	cubeShaderProgram.Init("shaders/05_hello_camera/camera.vert", "shaders/05_hello_camera/camera.frag");

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

	glEnable(GL_DEPTH_TEST);
	if (enableFaceCulling)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}
	glm::mat4 view = glm::mat4(1.0f);
#ifdef CAMERA_CONTROLS
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
#else
	float radius = 10.0f;
	float camX = sin(engine->GetTimeSinceInit()) * radius;
	float camZ = cos(engine->GetTimeSinceInit()) * radius;
	view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
#endif
#ifdef ORTHOGRAPHIC
	glm::mat4 projection = glm::ortho(
		-(float)config.screenSizeX / pixelPerUnit / 2.0f,
		(float)config.screenSizeX / pixelPerUnit / 2.0f,
		-(float)config.screenSizeY / pixelPerUnit / 2.0f,
		(float)config.screenSizeY / pixelPerUnit / 2.0f,
		0.1f, 100.0f);
#else
	glm::mat4 projection = glm::perspective(glm::radians(fov), 
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
	Engine* engine = Engine::GetPtr();
	auto& inputManager = engine->GetInputManager();
	float dt = engine->GetDeltaTime();
	float cameraSpeed = 1.0f;
#ifdef USE_SFML2
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
#endif

#ifdef USE_SDL2
	if (inputManager.GetButton(SDLK_w))
	{
		cameraPos += cameraSpeed * cameraFront * dt;
	}
	if (inputManager.GetButton(SDLK_s))
	{
		cameraPos -= cameraSpeed * cameraFront * dt;
	}
	if (inputManager.GetButton(SDLK_a))
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * dt;
	}
	if (inputManager.GetButton(SDLK_d))
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed* dt;
	}
#endif
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
