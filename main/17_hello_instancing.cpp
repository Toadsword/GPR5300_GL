#include <engine.h>
#include <graphics.h>
#include <model.h>
#include <camera.h>
#include "math_utility.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <Remotery.h>

//#define INSTANCING
class HelloInstancingDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void ProcessInput();
	void Draw() override;
	void Destroy() override;

	glm::vec3 CalculateForce();
	glm::vec3 CalculateVelocity();
private:
	Model asteroidModel{};
	Shader modelShader;
	const float asteroidScale = 0.01f;
	Camera camera = Camera(glm::vec3(0.0f, 100.0f, 500.0f));
	float lastX;
	float lastY;
	size_t planetNmb = 10'000;
	std::vector<glm::vec3> positions;
	std::vector<glm::mat4> modelMatrices;
	std::vector<glm::vec3> velocities;
#ifdef INSTANCING
	unsigned int instanceVBO;
#endif
};

void HelloInstancingDrawingProgram::Init()
{
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	lastX = config.screenWidth / 2.0f;
	lastY = config.screenHeight / 2.0f;

	programName = "Hello Instancing";

	asteroidModel.Init("data/models/rocks01/rock_01.obj");
	modelShader.CompileSource(
#ifdef INSTANCING
		"shaders/17_hello_instancing/model_instancing.vert", 
		"shaders/17_hello_instancing/model_instancing.frag"
#else
		"shaders/engine/model.vert",
		"shaders/engine/model.frag"
#endif
	);
	shaders.push_back(&modelShader);

	positions.resize(planetNmb);
	velocities.resize(planetNmb);
	modelMatrices.resize(planetNmb);

	for(auto i = 0u; i < planetNmb;i++)
	{
		const float radius = RandomRange(100, 300);
		const float angle = RandomRange(0, 359);
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);
		positions[i] = glm::rotate(position, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f) );
		positions[i] *= radius;

#ifdef INSTANCING
		glm::mat4& model = modelMatrices[i];
		model = glm::mat4(1.0f);
		model = glm::translate(model, positions[i]);
		model = glm::scale(model,
			glm::vec3(asteroidScale, asteroidScale, asteroidScale));
#endif

	}
#ifdef INSTANCING
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, planetNmb * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
	unsigned int VAO = asteroidModel.meshes[0].GetVAO();
	glBindVertexArray(VAO);
	GLsizei vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);

#endif
}

void HelloInstancingDrawingProgram::ProcessInput()
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

	float xoffset = mousePos.x - lastX;
	float yoffset = lastY - mousePos.y; // reversed since y-coordinates go from bottom to top
	lastX = mousePos.x;
	lastY = mousePos.y;

	camera.ProcessMouseMovement(xoffset, yoffset);

	camera.ProcessMouseScroll(inputManager.GetMouseWheelDelta());

}

void HelloInstancingDrawingProgram::Draw()
{
	rmt_ScopedOpenGLSample(DrawAsteroid);
	rmt_ScopedCPUSample(DrawAsteroidCPU, 0);
	glEnable(GL_DEPTH_TEST);
	ProcessInput();
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	glm::mat4 projection = glm::perspective(
		glm::radians(camera.Zoom),
		(float)config.screenWidth / (float)config.screenHeight,
		0.1f,
		1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	
	modelShader.Bind();
	modelShader.SetMat4("projection", projection);
	modelShader.SetMat4("view", view);
#ifdef INSTANCING
	Mesh& asteroidMesh = asteroidModel.meshes[0];
	for (unsigned int i = 0; i < asteroidMesh.textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = asteroidMesh.textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(1);
		else if (name == "texture_specular")
			number = std::to_string(1);

		modelShader.SetInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, asteroidMesh.textures[i].id);
}
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(asteroidMesh.GetVAO());
	glDrawElementsInstanced(
		GL_TRIANGLES, asteroidMesh.indices.size(), GL_UNSIGNED_INT, 0, planetNmb
	);
#else
	for (auto i = 0u; i < planetNmb; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, positions[i]);
		model = glm::scale(model,
			glm::vec3(asteroidScale, asteroidScale, asteroidScale));

		modelShader.SetMat4("model", model);
		asteroidModel.Draw(modelShader);
	}
#endif

}

void HelloInstancingDrawingProgram::Destroy()
{
}

glm::vec3 HelloInstancingDrawingProgram::CalculateForce()
{
	/*const auto deltaToCenter = screenSize / 2.0f - position;
	auto velDir = sf::Vector2f(-deltaToCenter.y, deltaToCenter.x);
	velDir /= Magnitude(velDir);

	const auto speed = sqrtf(Magnitude(CalculateNewForce(position)) / planetMass * pixel2meter(Magnitude(deltaToCenter)));
	return b2Vec2(velDir.x*speed, velDir.y*speed);*/
	return glm::vec3();
}

glm::vec3 HelloInstancingDrawingProgram::CalculateVelocity()
{
	/*const auto deltaToCenter = screenSize / 2.0f - position;
	const auto r = Magnitude(deltaToCenter);
	const auto force = gravityConst * centerMass*planetMass / (r*r);
	return pixel2meter(deltaToCenter / r * force);*/
	return glm::vec3();
}

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Instancing";

	engine.AddDrawingProgram(new HelloInstancingDrawingProgram());

	srand(0);
	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}
