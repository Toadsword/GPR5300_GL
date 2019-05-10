#include <engine.h>
#include <graphics.h>
#include "geometry.h"

#include <imgui.h>


class HelloParallaxDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void ProcessInput();
	void Draw() override;
	void Destroy() override;
	void UpdateUi() override;
private:
	unsigned int diffuseMap;
	unsigned int normalMap;
	unsigned int heightMap;
	Shader normalShader;
	Shader parallaxShader;
	Shader steepShader;
	Shader occlusionparallaxShader;
	Cube cube;
	Plane plane;
	float lightPos[3] = { 0,0,-9.5 };

	float heightScale = 0.06f;

	int parallaxStep = 1;
};

void HelloParallaxDrawingProgram::Init()
{
	programName = "Parallax Map";

	normalShader.CompileSource(
		"shaders/50_hello_parallax/normal.vert",
		"shaders/50_hello_parallax/normal.frag"
	);
	parallaxShader.CompileSource(
		"shaders/50_hello_parallax/parallax.vert",
		"shaders/50_hello_parallax/parallax.frag"
	);
	steepShader.CompileSource(
		"shaders/50_hello_parallax/parallax.vert",
		"shaders/50_hello_parallax/steepParallax.frag"
	);
	occlusionparallaxShader.CompileSource(
		"shaders/50_hello_parallax/parallax.vert",
		"shaders/50_hello_parallax/occlusionParallax.frag"
	);
	shaders.push_back(&normalShader);
	shaders.push_back(&parallaxShader);
	shaders.push_back(&steepShader);
	shaders.push_back(&occlusionparallaxShader);

	//cube.Init();
	plane.Init();

	diffuseMap = stbCreateTexture("data/sprites/bricks2.jpg");
	normalMap = stbCreateTexture("data/sprites/bricks2_normal.jpg");
	heightMap = stbCreateTexture("data/sprites/bricks2_disp.jpg");
}

void HelloParallaxDrawingProgram::ProcessInput()
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

	auto mousePos = inputManager.GetMousePosition();

	camera.ProcessMouseMovement(mousePos.x, mousePos.y, true);

	camera.ProcessMouseScroll(inputManager.GetMouseWheelDelta());
}

void HelloParallaxDrawingProgram::Draw()
{
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	ProcessInput();
	glEnable(GL_DEPTH_TEST);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 100.0f);
	Shader& currentShader = Shader();
	switch(parallaxStep)
	{
		case 1 :
			currentShader = normalShader;
		break;
		case 2:
			currentShader = parallaxShader;
			break;
		case 3:
			currentShader = steepShader;
			break;
		case 4:
			currentShader = occlusionparallaxShader;
			break;
		default:
			break;
	}
	//Shader& currentShader = enableParallax ? parallaxShader : normalShader;
	currentShader.Bind();
	currentShader.SetMat4("model", model);
	currentShader.SetMat4("view", camera.GetViewMatrix());
	currentShader.SetMat4("projection", projection);

	currentShader.SetVec3("viewPos", camera.Position);
	currentShader.SetVec3("lightPos", lightPos);

	currentShader.SetFloat("heightScale", heightScale);

	currentShader.SetInt("diffuseMap", 0);
	currentShader.SetInt("normalMap", 1);
	currentShader.SetInt("depthMap", 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightMap);

	//cube.Draw();
	plane.Draw();
}

void HelloParallaxDrawingProgram::Destroy()
{
}

void HelloParallaxDrawingProgram::UpdateUi()
{
	ImGui::Separator();
	ImGui::InputFloat3("LightPos", lightPos);
	ImGui::SliderFloat("HeightScale", &heightScale, 0.0f, 1.0f);
	ImGui::SliderInt("Step Parallax", &parallaxStep, 1, 4);
	ImGui::Text("1 : Normal Mapping");
	ImGui::Text("2 : Parallax Mapping");
	ImGui::Text("3 : Steep Parallax Mapping");
	ImGui::Text("4 : Occlusion Parallax Mapping");
}

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Parallax";
	config.bgColor = { 0,0,0,0 };
	engine.AddDrawingProgram(new HelloParallaxDrawingProgram());

	srand(0);
	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}
