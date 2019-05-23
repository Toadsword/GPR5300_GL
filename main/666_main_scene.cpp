
#include <algorithm>

#include <engine.h>
#include <graphics.h>
#include <geometry.h>

#include <imgui.h>


#define Camera
#ifdef  Camera
class CameraProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
	void ProcessInput();
};

void CameraProgram::Init()
{
	programName = "Camera";
}

void CameraProgram::Draw()
{
	ProcessInput();
}

void CameraProgram::Destroy()
{
}

void CameraProgram::ProcessInput()
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
#endif

#define Firefly
#ifdef Firefly
// CPU representation of a particle
struct FireflyParticle {
	glm::vec3 startPos, destPos, diffPos;
	float timeSinceBegin, timeToEnd, cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const FireflyParticle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

const int MaxParticles = 100;

class FireflyDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;

	void UpdateUi() override;

	int ProcessParticles(float dt);
	void SortParticles();

private:
	//Consts (Editable by Imgui)
	int NumFireflies = MaxParticles;
	float botRightLimit[3] = { -10.0f, -10.0f, -10.0f };
	int range[3] = { 40, 40, 40 };

	Shader fireflyShaderProgram;
	Shader blurShader;
	Shader hdrShader;

	Plane hdrPlane;
	unsigned hdrFBO = 0;
	unsigned rboDepth = 0;
	unsigned hdrColorBuffer[2];

	GLuint pingpongFBO[2];
	GLuint pingpongBuffer[2];

	unsigned fireflyTexture;

	unsigned VAO;
	unsigned VBO[2];
	unsigned EBO;

	GLfloat vertices[12] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left
	};
	GLfloat texCoords[8] = {
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

	std::vector<FireflyParticle> ParticlesContainer;
	int LastUsedParticle = 0;
	float exposure = 2.1f;

	
	float particlesPosition[3 * MaxParticles];
	float particlesColor[3 * MaxParticles];
	// 1 for scale (for the 3 axis)
	// 1 for glowingEffect
	float particlesScale[3 * MaxParticles];

	unsigned int particlesPositionBuffer;
	unsigned int particlesColorBuffer;
	unsigned int particlesScaleBuffer;
};

void FireflyDrawingProgram::Init()
{
	programName = "Firefly";

	auto* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();

	ParticlesContainer.resize(NumFireflies);
	//Init particles
	for (int i = 0; i < NumFireflies; i++) {
		ParticlesContainer[i].destPos = glm::vec3(
			rand() % 20 + botRightLimit[0],
			rand() % 20 + botRightLimit[1],
			rand() % 20 + botRightLimit[2]
		);

		ParticlesContainer[i].timeSinceBegin = -1.0f;
		ParticlesContainer[i].timeToEnd = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;

		LastUsedParticle = i;

		// Buffering init values for each particle
		//float exposure = rand() % 64;
		particlesColor[3 * i + 0] = (rand() % 32) / 256.0f;	// color (r)
		particlesColor[3 * i + 1] = (rand() % 78) / 256.0f;	// color (g)
		particlesColor[3 * i + 2] = (rand() % 32) / 256.0f;	// color (b)

		particlesScale[3 * i + 0] = (rand() % 4) / 5.0f + 0.8f;	// scale (for the 3 axis)
		particlesScale[3 * i + 1] = std::max((rand() % 10) / 10.0f - 0.3f, 0.2f) / 3.0f; // glowScale
		particlesScale[3 * i + 2] = (rand() % 6 + 4.0f) / 2.0f; // glowSpeed
	}

	fireflyShaderProgram.CompileSource("shaders/666_main_scene/firefly.vert", "shaders/666_main_scene/firefly.frag"); // quad
	shaders.push_back(&fireflyShaderProgram);

	hdrShader.CompileSource("shaders/666_main_scene/hdr.vert", "shaders/666_main_scene/hdr.frag");
	shaders.push_back(&hdrShader);
	
	blurShader.CompileSource("shaders/666_main_scene/hdr.vert", "shaders/666_main_scene/blur.frag");

	fireflyTexture = stbCreateTexture("data/sprites/firefly5.png", true, true, true);

	//////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////			BINDING HDR 		//////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////

	hdrPlane.Init();
	glGenFramebuffers(1, &hdrFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glGenTextures(2, hdrColorBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, hdrColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,
			config.screenWidth, config.screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D, hdrColorBuffer[i], 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32,
		config.screenWidth,
		config.screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
		rboDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, config.screenWidth, config.screenHeight, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		);
	}

	//////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////			BINDING VAO			//////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////

	glGenBuffers(2, &VBO[0]);
	glGenBuffers(1, &particlesColorBuffer);
	glGenBuffers(1, &particlesScaleBuffer);
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO); //like: new VAO()
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);//Now use our VAO
	//bind vertices data
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//bind texture coords data
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	//bind vertices index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//bind particle Color
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particlesColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(float), particlesColor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Size of Data
	//glBufferSubData(GL_ARRAY_BUFFER, 0, MaxParticles * 4 * sizeof(float), &particleData);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, particlesScaleBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(float), particlesScale, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Size of Data
	//glBufferSubData(GL_ARRAY_BUFFER, 0, MaxParticles * 4 * sizeof(float), &particleData);

	//bind particle position data
	glEnableVertexAttribArray(4);
	glGenBuffers(1, &particlesPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particlesPositionBuffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(float), NULL, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Size of Position

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	//unbind Vertex Array
	glBindVertexArray(0);
}

void FireflyDrawingProgram::Draw()
{
	Engine* engine = Engine::GetPtr();
	Configuration& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();
	float dt = engine->GetDeltaTime();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Bind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	fireflyShaderProgram.Bind();

	// Bind our texture in Texture Unit 0
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fireflyTexture);
	glBindVertexArray(VAO);

	const int ParticleCount = ProcessParticles(dt);

	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / config.screenHeight, 0.1f, 100.0f);

	// Same as the billboards tutorial
	fireflyShaderProgram.SetVec3("CameraRight", viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	fireflyShaderProgram.SetVec3("CameraUp", viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
	fireflyShaderProgram.SetMat4("VP", projection * viewMatrix);
	fireflyShaderProgram.SetFloat("time", engine->GetTimeSinceInit());

	//Update the buffer with all the positions
	glBindBuffer(GL_ARRAY_BUFFER, particlesPositionBuffer);
	//glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticleCount * 3 * sizeof(float), &particlesPosition);

	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
	// but faster.
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ParticleCount);

	bool horizontal = true, first_iteration = true;
	int amount = 10;
	blurShader.Bind();
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		blurShader.SetInt("horizontal", horizontal);
		blurShader.SetInt("image", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? hdrColorBuffer[1] : pingpongBuffer[!horizontal]
		);
		hdrPlane.Draw();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	//Show hdr quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	hdrShader.Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrColorBuffer[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
	hdrShader.SetInt("hdrBuffer", 0);
	hdrShader.SetInt("bloomBlur", 1);
	hdrShader.SetFloat("exposure", exposure);
	hdrPlane.Draw();

	glBindVertexArray(0);
}

void FireflyDrawingProgram::Destroy()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glDeleteBuffers(1, &particlesPositionBuffer);

	glDeleteTextures(1, &fireflyTexture);

	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(2, &EBO);
}

void FireflyDrawingProgram::UpdateUi()
{
	ImGui::Separator();
	ImGui::InputInt3("Limite top left", range);
	ImGui::InputFloat3("Limite bottom right", botRightLimit);
	ImGui::SliderInt("Num fireflies", &NumFireflies, 1, MaxParticles);
	ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
	ImGui::Separator();
}

int FireflyDrawingProgram::ProcessParticles(float dt)
{
	// Simulate all particles
	int ParticlesCount = 0;
	for (int i = 0; i < NumFireflies; i++) 
	{
		FireflyParticle& p = ParticlesContainer[i]; // shortcut

		p.timeSinceBegin -= dt;
		if(p.timeSinceBegin < 0.0f)
		{
			p.startPos = p.destPos;
			p.destPos = glm::vec3(
				rand() % range[0] + botRightLimit[0],
				rand() % range[1] + botRightLimit[1],
				rand() % range[2] + botRightLimit[2]
			);
			p.diffPos = p.destPos - p.startPos;

			p.timeToEnd = glm::length(p.diffPos) / (rand() % 4 * 1.0f + 1);
			p.timeSinceBegin = p.timeToEnd;
		}

		//Parametric blend, ease in and out. Results in this : https://www.wolframalpha.com/input/?i=t%5E2(3-2t)
		// Go from 0 to 1. (due to 1 - ...)
		float t = 1 - p.timeSinceBegin / p.timeToEnd;
		float change = pow(t, 2) / (2.0f * (pow(t, 2) - t) + 1.0f);


		// Fill the GPU buffer
		particlesPosition[3 * i + 0] = p.startPos.x + p.diffPos.x * change;
		particlesPosition[3 * i + 1] = p.startPos.y + p.diffPos.y * change;
		particlesPosition[3 * i + 2] = p.startPos.z + p.diffPos.z * change;

		ParticlesCount++;
	}

	SortParticles();

	return ParticlesCount;
}

void FireflyDrawingProgram::SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles - 1]);
}
#endif

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Main Scene";
	config.bgColor = { 1,1,1,1 };

#ifdef Camera
	engine.AddDrawingProgram(new CameraProgram());
#endif
#ifdef Firefly
	engine.AddDrawingProgram(new FireflyDrawingProgram());
#endif

	engine.Init();

	engine.GameLoop();
	return EXIT_SUCCESS;
}