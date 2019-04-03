#if WIN32
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include <engine.h>
#include <graphics.h>
#include <camera.h>


#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

const float pixelPerUnit = 100.0f;
//#define ORTHOGRAPHIC
#define CAMERA_CONTROLS

class HelloStencilDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
private:
	void ProcessInput();
public:

private:
	Shader cubeShaderProgram;
	Shader floorShaderProgram;
	unsigned int cubeVBO, cubeVAO;
	unsigned int floorVBO, floorVAO;
	unsigned int textureWall;
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
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	float floorVertices[6*6] = {

	-1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 
	 1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 
	 1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 
	 1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f,
	-1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 
	-1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 
	};
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = 0;
	float lastY = 0;
};


void HelloStencilDrawingProgram::Init()
{
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	lastX = config.screenWidth / 2.0f;
	lastY = config.screenHeight / 2.0f;

	programName = "Hello Stencil";
	shaders.push_back(&cubeShaderProgram);
	shaders.push_back(&floorShaderProgram);
	cubeShaderProgram.Init("shaders/11_hello_stencil/cube.vert", "shaders/11_hello_stencil/cube.frag");
	floorShaderProgram.Init("shaders/11_hello_stencil/floor.vert", "shaders/11_hello_stencil/floor.frag");
	textureWall = stbCreateTexture("data/sprites/wall.jpg");
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

	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);

	glBindVertexArray(floorVAO);

	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

}

void HelloStencilDrawingProgram::Draw()
{
	ProcessInput();

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();

	glEnable(GL_DEPTH_TEST);

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 100.0f);


	cubeShaderProgram.Bind();
	int viewLoc = glGetUniformLocation(cubeShaderProgram.GetProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	int projectionLoc = glGetUniformLocation(cubeShaderProgram.GetProgram(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(glGetUniformLocation(cubeShaderProgram.GetProgram(), "overrideColor"), 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, textureWall);
	glBindVertexArray(cubeVAO);
	
	glm::mat4 model = glm::mat4(1.0f);
	
	int modelLoc = glGetUniformLocation(cubeShaderProgram.GetProgram(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glEnable(GL_STENCIL_TEST);

	
	// Draw floor
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF); // Write to stencil buffer
	glDepthMask(GL_FALSE); // Don't write to depth buffer
	glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
	floorShaderProgram.Bind();

	viewLoc = glGetUniformLocation(floorShaderProgram.GetProgram(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	projectionLoc = glGetUniformLocation(floorShaderProgram.GetProgram(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindTexture(GL_TEXTURE_2D, textureWall);
	glBindVertexArray(cubeVAO);


	modelLoc = glGetUniformLocation(floorShaderProgram.GetProgram(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	// Draw cube reflection
	
	glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
	glStencilMask(0x00); // Don't write anything to stencil buffer
	glDepthMask(GL_TRUE); // Write to depth buffer

	model = glm::scale(
		glm::translate(model, glm::vec3(0, 0, -1)),
		glm::vec3(1, 1, -1)
	);
	cubeShaderProgram.Bind();
	glBindVertexArray(cubeVAO);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(glGetUniformLocation(cubeShaderProgram.GetProgram(), "overrideColor"), 0.3f, 0.3f, 0.3f);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glUniform3f(glGetUniformLocation(cubeShaderProgram.GetProgram(), "overrideColor"), 1.0f, 1.0f, 1.0f);
	glDisable(GL_STENCIL_TEST);
}

void HelloStencilDrawingProgram::Destroy()
{
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
}

void HelloStencilDrawingProgram::ProcessInput()
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
	config.windowName = "Hello Camera";
	config.bgColor = { 255,255,255 };

	engine.AddDrawingProgram(new HelloStencilDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}
