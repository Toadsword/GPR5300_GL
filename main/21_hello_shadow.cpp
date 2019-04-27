#include <engine.h>
#include <graphics.h>
#include <geometry.h>
#include <model.h>
#include "light.h"

class HelloShadowDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
private:
	void DrawScene(bool depthOnly = false);
	Shader basicShader;
	Shader modelShader;

	Model floorPlane;
	Model humanModel;
	Model rockModel;

	DirectionLight directionLight;
	unsigned int floorTexture;

	unsigned int depthMapFBO;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
};

void HelloShadowDrawingProgram::Init()
{
	programName = "Hello Shadow";
	auto engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();

	camera.Position = glm::vec3(0.0f, 2.0f, 10.0f);

	floorPlane.Init("data/models/granit01/granit.obj");
	floorTexture = stbCreateTexture("data/models/granit01/granite_01_dif.tga");
	humanModel.Init("data/models/nanosuit2/nanosuit.obj");
	rockModel.Init("data/models/rocks01/rock_01.obj");

	basicShader.CompileSource("shaders/engine/basic.vert", 
		"shaders/engine/basic.frag");
	modelShader.CompileSource("shaders/engine/model.vert",
		"shaders/engine/model.frag");
	shaders.push_back(&modelShader);


	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HelloShadowDrawingProgram::Draw()
{
	auto* engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();
	auto& config = engine->GetConfiguration();

	ProcessInput();

	glEnable(GL_DEPTH_TEST);

	DrawScene();
}

void HelloShadowDrawingProgram::Destroy()
{
}

void HelloShadowDrawingProgram::DrawScene(bool depthOnly)
{
	auto* engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();
	auto& config = engine->GetConfiguration();

	Shader& currentShader = depthOnly ? basicShader : modelShader;
	currentShader.Bind();
	glm::mat4 projection = glm::perspective(
		camera.Zoom,
		(float)config.screenWidth / config.screenHeight,
		0.1f, 100.0f);
	currentShader.SetMat4("projection", projection);
	currentShader.SetMat4("view", camera.GetViewMatrix());

	currentShader.SetBool("directionalLightEnable", true);
	currentShader.SetVec3("directionLight.direction", glm::normalize(
		glm::vec3(1.0f, -1.0f, 0.0f)));
	currentShader.SetFloat("directionLight.intensity", 1.0f);

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::scale(modelMat, glm::vec3(0.2f, 0.2f, 0.2f));
	currentShader.SetMat4("model", modelMat);
	humanModel.Draw(currentShader);

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(4.0f, 0.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.01f, 0.01f, 0.01f));
	currentShader.SetMat4("model", modelMat);
	rockModel.Draw(currentShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	modelMat = glm::mat4(1.0f);
	modelMat = glm::scale(modelMat, glm::vec3(5.0f, 5.0f, 5.0f));
	currentShader.SetMat4("model", modelMat);
	floorPlane.Draw(currentShader);
}

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Shadow";
	config.bgColor = { 1,1,1,1 };

	engine.AddDrawingProgram(new HelloShadowDrawingProgram());

	srand(0);
	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}
