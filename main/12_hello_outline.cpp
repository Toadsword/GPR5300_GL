#include <engine.h>
#include <graphics.h>
#include <model.h>

#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"


class HelloOutlineDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
	void ProcessInput();
	void UpdateUi() override;

private:
	Shader modelShaderProgram;
	Shader outlineShaderProgram;

	unsigned int VBO = 0, cubeVAO = 0;

	Model model;

	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = 0;
	float lastY = 0;

	float outlineMoveScale = 0.1f;
	float outlineColor[3] = { 1.0f,1.0f,1.0f };
};

void HelloOutlineDrawingProgram::Init()
{
	programName = "Hello Outline";

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	lastX = config.screenWidth / 2.0f;
	lastY = config.screenHeight / 2.0f;

	modelShaderProgram.Init(
		"shaders/12_hello_outline/model.vert",
		"shaders/12_hello_outline/model.frag");
	outlineShaderProgram.Init(
		"shaders/12_hello_outline/outline.vert",
		"shaders/12_hello_outline/outline.frag");
	shaders.push_back(&modelShaderProgram);
	shaders.push_back(&outlineShaderProgram);
	// "data/models/nanosuit/scene.fbx"
	// "data/models/van_gogh_room/Enter a title.obj"
	// 
	model.Init("data/models/nanosuit2/nanosuit.obj");
}

void HelloOutlineDrawingProgram::Draw()
{
	ProcessInput();

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();

	glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should update the stencil buffer
	glStencilMask(0xFF); // enable writing to the stencil buffer
	glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
	modelShaderProgram.Bind();

	modelShaderProgram.SetMat4("view", view);
	modelShaderProgram.SetMat4("projection", projection);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down

	const int modelLoc = glGetUniformLocation(modelShaderProgram.GetProgram(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	modelShaderProgram.SetMat4("model", model);
	this->model.Draw(modelShaderProgram);
	glStencilMask(0xFF); // disable writing to the stencil buffer
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00); // disable writing to the stencil buffer
	glDisable(GL_DEPTH_TEST);

	outlineShaderProgram.Bind();
	outlineShaderProgram.SetMat4("view", view);
	outlineShaderProgram.SetMat4("projection", projection);
	outlineShaderProgram.SetVec3("outlineColor", outlineColor);

	glm::vec3 sides [8]=
	{
		glm::vec3(1.0,0.0,0.0),
		glm::vec3(1.0,1.0,0.0),
		glm::vec3(0.0,1.0,0.0),
		glm::vec3(-1.0,1.0,0.0),
		glm::vec3(-1.0,0.0,0.0),
		glm::vec3(-1.0,-1.0,0.0),
		glm::vec3(0.0,-1.0,0.0),
		glm::vec3(1.0,-1.0,0.0),
	};
	for (auto side : sides)
	{
		const auto newModel = glm::translate(model, side*outlineMoveScale);
		outlineShaderProgram.SetMat4("model", newModel);
		this->model.Draw(outlineShaderProgram);
	}
	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

}

void HelloOutlineDrawingProgram::Destroy()
{
}

void HelloOutlineDrawingProgram::UpdateUi()
{
	ImGui::Separator();
	ImGui::InputFloat("moveScale", &outlineMoveScale);
	ImGui::ColorEdit3("outlineColor", outlineColor);
}




void HelloOutlineDrawingProgram::ProcessInput()
{
	Engine* engine = Engine::GetPtr();
	auto& inputManager = engine->GetInputManager();
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

	auto mousePos = inputManager.GetMousePosition();

	float xOffset = mousePos.x - lastX;
	float yOffset = lastY - mousePos.y; // reversed since y-coordinates go from bottom to top
	lastX = mousePos.x;
	lastY = mousePos.y;

	camera.ProcessMouseMovement(xOffset, yOffset);

	camera.ProcessMouseScroll(inputManager.GetMouseWheelDelta());


}


int main(int argc, char** argv)
{
	Engine engine;
	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Outline";
	engine.AddDrawingProgram(new HelloOutlineDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}