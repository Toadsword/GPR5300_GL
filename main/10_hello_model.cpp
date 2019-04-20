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


class HelloOutlineDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
	void ProcessInput();

private:
	Shader modelShaderProgram;



	Model model;

	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = 0;
	float lastY = 0;
};

void HelloOutlineDrawingProgram::Init()
{
	programName = "Hello Model";

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	lastX = config.screenWidth / 2.0f;
	lastY = config.screenHeight / 2.0f;

    modelShaderProgram.CompileSource(
            "shaders/10_hello_model/model.vert",
            "shaders/10_hello_model/model.frag");
	shaders.push_back(&modelShaderProgram);
	// "data/models/nanosuit2/nanosuit.obj"
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
	glEnable(GL_CULL_FACE);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	modelShaderProgram.Bind();
	const int viewLoc = glGetUniformLocation(modelShaderProgram.GetProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	const int projectionLoc = glGetUniformLocation(modelShaderProgram.GetProgram(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down

	const int modelLoc = glGetUniformLocation(modelShaderProgram.GetProgram(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	this->model.Draw(modelShaderProgram);

}

void HelloOutlineDrawingProgram::Destroy()
{
}



void HelloOutlineDrawingProgram::ProcessInput()
{
	Engine* engine = Engine::GetPtr();
	auto& inputManager = engine->GetInputManager();
	float dt = engine->GetDeltaTime();
	float cameraSpeed = 1.0f;
#ifdef USE_SFML2
	if (inputManager.GetButton(sf::Keyboard::W))
	{
		camera.ProcessKeyboard(FORWARD, engine->GetDeltaTime());
	}
	if (inputManager.GetButton(sf::Keyboard::S))
	{
		camera.ProcessKeyboard(BACKWARD, engine->GetDeltaTime());
	}
	if (inputManager.GetButton(sf::Keyboard::A))
	{
		camera.ProcessKeyboard(LEFT, engine->GetDeltaTime());
	}
	if (inputManager.GetButton(sf::Keyboard::D))
	{
		camera.ProcessKeyboard(RIGHT, engine->GetDeltaTime());
	}
#endif

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

	float xoffset = mousePos.x - lastX;
	float yoffset = lastY - mousePos.y; // reversed since y-coordinates go from bottom to top
	lastX = mousePos.x;
	lastY = mousePos.y;

	camera.ProcessMouseMovement(xoffset, yoffset);

	camera.ProcessMouseScroll(inputManager.GetMouseWheelDelta());


}


int main(int argc, char** argv)
{
	Engine engine;
	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Model";
	engine.AddDrawingProgram(new HelloOutlineDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}