#include <engine.h>
#include <graphics.h>
#include <model.h>

#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
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

};

void HelloOutlineDrawingProgram::Init()
{
	programName = "Hello Model";

    modelShaderProgram.CompileSource(
            "shaders/10_hello_model/model.vert",
            "shaders/10_hello_model/model.frag");
	shaders.push_back(&modelShaderProgram);
	// "data/models/nanosuit2/nanosuit.obj"
	// "data/models/van_gogh_room/Enter a title.obj"
	// 
	model.Init("data/models/nanosuit2/nanosuit.blend");
}

void HelloOutlineDrawingProgram::Draw()
{
	ProcessInput();

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	modelShaderProgram.Bind();
	modelShaderProgram.SetMat4("view", view);
	modelShaderProgram.SetMat4("projection", projection);
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down

	modelShaderProgram.SetMat4("model", model);
	
	this->model.Draw(modelShaderProgram);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -20.75f, 20.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down

	modelShaderProgram.SetMat4("model", model);

	this->model.Draw(modelShaderProgram);
}

void HelloOutlineDrawingProgram::Destroy()
{
}


void HelloOutlineDrawingProgram::ProcessInput()
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
	config.windowName = "Hello Model";
	engine.AddDrawingProgram(new HelloOutlineDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}