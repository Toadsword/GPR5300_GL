#include <map>
#include <engine.h>
#include <graphics.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TRANSPARENT
#define BLENDING
#define ORDER

class HelloBillboardDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;

	void ProcessInput();

private:
	Shader windowShaderProgram;
	Shader grassShaderProgram;

	unsigned VAO;
	unsigned grassTexture;
	unsigned windowTexture;

	unsigned VBO[2];
	unsigned EBO;
	float vertices[12] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left
	};
	float texCoords[8] = {
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

	const static int grassNmb = 3;
	glm::vec3 grassPositions [grassNmb] =
	{
		glm::vec3(1., 0.0,-3.0),
		glm::vec3(1., 0.0,-1.0),
		glm::vec3(-1.,0.0,-2.0),
	};

	const static int windowNmb = 3;

	glm::vec3 windowPositions [windowNmb] =
	{
		glm::vec3(-1., 0.0,-3.0),
		glm::vec3(-1., 0.0,-3.0),
		glm::vec3(1., 0.0, -2.0),
	};

	float windowAngles [windowNmb] =
	{
		glm::radians(45.0f),
		glm::radians(-45.0f),
		0.0f
	};

};

void HelloBillboardDrawingProgram::Init()
{
	programName = "HelloBillBoard";

	shaders.push_back(&grassShaderProgram);
	shaders.push_back(&windowShaderProgram);
#ifdef BLENDING
    windowShaderProgram.CompileSource(
            "shaders/51_hello_billboard/quad.vert",
            "shaders/51_hello_billboard/quad.frag");
#else

#ifdef TRANSPARENT
	windowShaderProgram.CompileSource(
		"shaders/51_hello_billboard/quad.vert",
		"shaders/51_hello_billboard/full_transparent.frag");
#else
	windowShaderProgram.CompileSource(
		"shaders/51_hello_billboard/quad.vert",
		"shaders/51_hello_billboard/quad.frag");
#endif

#endif

#ifdef TRANSPARENT
    grassShaderProgram.CompileSource(
            "shaders/51_hello_billboard/billboard.vert",
            "shaders/51_hello_billboard/billboard.frag"); // quad
#else
	grassShaderProgram.CompileSource(
		"shaders/51_hello_billboard/quad.vert",
		"shaders/51_hello_billboard/quad.frag");
#endif

	grassTexture = stbCreateTexture("data/sprites/grass.png", true, true, true);
	windowTexture = stbCreateTexture("data/sprites/blending_transparent_window.png");

	glGenBuffers(2, &VBO[0]);
	glGenBuffers(1, &EBO);


	glGenVertexArrays(1, &VAO); //like: new VAO()
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);//Now use our VAO
	//bind vertices data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//bind texture coords data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	//bind vertices index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//unbind Vertex Array
	glBindVertexArray(0);

}

void HelloBillboardDrawingProgram::Draw()
{
	ProcessInput();

	glEnable(GL_DEPTH_TEST);
	Engine* engine = Engine::GetPtr();
	Configuration& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / config.screenHeight, 0.1f, 100.0f);

	glm::mat4 viewProjMatrix = view * projection;

	//Draw grass
	grassShaderProgram.Bind();
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glBindVertexArray(VAO);

	//grassShaderProgram.SetMat4("view", view);
	//grassShaderProgram.SetMat4("projection", projection);
	grassShaderProgram.SetMat4("VP", viewProjMatrix);
	grassShaderProgram.SetVec3("CameraRight", view[0][0], view[1][0], view[2][0]);
	grassShaderProgram.SetVec3("CameraUp", view[0][1], view[1][1], view[2][1]);
	//grassShaderProgram.SetVec2("BillboardSize", 1.0f, 1.0f);
	//grassShaderProgram.SetVec3("color", glm::vec4(0.2f, 0.1f, 0.1f, 1.0f));

	//glm::mat4 model = glm::mat4(1.0f); //model transform matrix

	for (auto grassPosition : grassPositions)
	{
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, grassPosition);
		//grassShaderProgram.SetMat4("model", model);
		grassShaderProgram.SetVec3("BillboardPos", grassPosition);
		//grassShaderProgram.SetVec3("offset", grassPosition);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	//Draw window
#ifdef BLENDING
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
	windowShaderProgram.Bind();
	glBindTexture(GL_TEXTURE_2D, windowTexture);

	//windowShaderProgram.SetMat4("view", view);
	windowShaderProgram.SetMat4("projection", viewProjMatrix);
	//windowShaderProgram.SetMat4("VP", viewProjMatrix);
	//windowShaderProgram.SetVec3("CameraRight_worldspace", view[0][0], view[1][0], view[2][0]);
	//windowShaderProgram.SetVec3("CameraUp_worldspace", view[0][1], view[1][1], view[2][1]);
	//windowShaderProgram.SetVec3("color", glm::vec4(0.2f, 0.1f, 0.1f, 1.0f));

#ifdef ORDER
	std::map<float, glm::vec3> sorted;
	for (auto windowPosition : windowPositions)
	{
		float distance = glm::length(camera.Position - windowPosition);
		sorted[distance] = windowPosition;
	}
	for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		auto windowPosition = it->second;
#else
	for (int i = 0; i < windowNmb;i++)
	{
		auto windowPosition = windowPositions[i];
#endif
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, windowPosition);
		//model = glm::rotate(model, windowAngles[i], glm::vec3(0,1,0));

		//windowShaderProgram.SetVec3("BillboardPos", windowPosition);
		//windowShaderProgram.SetVec2("BillboardSize", 1.0f, 1.0f);
		windowShaderProgram.SetVec3("offset", -camera.Position + windowPosition);

		//model = glm::lookAt(windowPosition, camera.Position + camera.Up, camera.Up);
		//windowShaderProgram.SetMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}

void HelloBillboardDrawingProgram::Destroy()
{
	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(2, &EBO);
}

void HelloBillboardDrawingProgram::ProcessInput()
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
	config.windowName = "Hello Blending";
	config.bgColor = { 0,0,0 };

	engine.AddDrawingProgram(new HelloBillboardDrawingProgram());

	engine.Init();

	engine.GameLoop();
	return EXIT_SUCCESS;
}
