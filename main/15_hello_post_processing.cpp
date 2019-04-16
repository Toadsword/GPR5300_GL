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
#include <iostream>

enum class PostProcessFx
{
	None,
	Inversion,
	Grayscale,
	Kernel,
	Blur,
	EdgeDetection
};

class HelloPostProcessDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void DrawScene();
	void Draw() override;
	void Destroy() override;
	void ProcessInput();

private:
	Shader frameBufferShaderProgram;
	Shader cubeShaderProgram;
	unsigned int textureWall;
	unsigned int cubeVBO = 0, cubeVAO = 0;
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
	float quadVertices[4*6] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = 0;
	float lastY = 0;


	unsigned int texColorBuffer;
	unsigned int framebuffer;
	unsigned int rbo;

	unsigned int quadVAO, quadVBO;
	glm::mat4 projection;
	glm::mat4 view;
	PostProcessFx postProcessingFx = PostProcessFx::Blur;
};

void HelloPostProcessDrawingProgram::Init()
{
	programName = "Hello Post Processing";
	
	//Cube
	cubeShaderProgram.Init("shaders/05_hello_camera/camera.vert", "shaders/05_hello_camera/camera.frag");
	shaders.push_back(&cubeShaderProgram);
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
	//screen quad
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	lastX = config.screenWidth / 2.0f;
	lastY = config.screenHeight / 2.0f;
	//Framebuffer
	switch(postProcessingFx)
	{
	case PostProcessFx::None:
		frameBufferShaderProgram.Init(
			"shaders/15_hello_post_processing/frame.vert",
			"shaders/15_hello_post_processing/frame.frag"
		);
		break;
	case PostProcessFx::Inversion:
		frameBufferShaderProgram.Init(
			"shaders/15_hello_post_processing/frame.vert",
			"shaders/15_hello_post_processing/inversion.frag"
		);
		break;
	case PostProcessFx::Grayscale:
		frameBufferShaderProgram.Init(
			"shaders/15_hello_post_processing/frame.vert",
			"shaders/15_hello_post_processing/grayscale.frag"
		);
		break;
	case PostProcessFx::Kernel:
		frameBufferShaderProgram.Init(
			"shaders/15_hello_post_processing/frame.vert",
			"shaders/15_hello_post_processing/kernel.frag"
		);
		break;
	case PostProcessFx::Blur:
		frameBufferShaderProgram.Init(
			"shaders/15_hello_post_processing/frame.vert",
			"shaders/15_hello_post_processing/blur.frag"
		);
		break;
	case PostProcessFx::EdgeDetection:
		frameBufferShaderProgram.Init(
			"shaders/15_hello_post_processing/frame.vert",
			"shaders/15_hello_post_processing/edge_detection.frag"
		);
		break;
	default:
		break;
	}
	
	shaders.push_back(&frameBufferShaderProgram);
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	//Generate texture for framebuffer
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, config.screenWidth, config.screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	
	//Render Buffer Object
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, config.screenWidth, config.screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "[ERROR] Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HelloPostProcessDrawingProgram::DrawScene()
{
	Engine* engine = Engine::GetPtr();
	cubeShaderProgram.Bind();
	cubeShaderProgram.SetMat4("view", view);
	cubeShaderProgram.SetMat4("projection", projection);

	glBindTexture(GL_TEXTURE_2D, textureWall);
	glBindVertexArray(cubeVAO);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f,0.0f,-1.0f));
	cubeShaderProgram.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, 0.0f, -1.0f));
	cubeShaderProgram.SetMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(0);
}

void HelloPostProcessDrawingProgram::Draw()
{
	ProcessInput();

	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	projection = glm::perspective(glm::radians(camera.Zoom), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	// first pass
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	glEnable(GL_DEPTH_TEST);

	DrawScene();

	// second pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default screen framebuffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	frameBufferShaderProgram.Bind();
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);


}

void HelloPostProcessDrawingProgram::Destroy()
{
}



void HelloPostProcessDrawingProgram::ProcessInput()
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


int main(int argc, char** argv)
{
	Engine engine;
	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Post Processing";
	engine.AddDrawingProgram(new HelloPostProcessDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}