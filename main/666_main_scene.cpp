
#include <algorithm>

#include <engine.h>
#include <graphics.h>
#include <geometry.h>


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
	glm::vec3 pos, speed, topLeftLimit, bottomRightLimit;
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

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

	int ProcessParticles(float dt);
	void SortParticles();

private:
	Shader fireflyShaderProgram;
	Shader blurShader;

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

	unsigned int instanceVBO;

	std::vector<FireflyParticle> ParticlesContainer;
	int LastUsedParticle = 0;

	float particle_position[3 * MaxParticles];

	unsigned int particles_position_buffer;
};

void FireflyDrawingProgram::Init()
{
	programName = "Firefly";

	auto* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();

	ParticlesContainer.resize(MaxParticles);
	for (int i = 0; i < MaxParticles; i++) {
		ParticlesContainer[i].cameradistance = -1.0f;
		ParticlesContainer[i].speed = glm::vec3(-1.0f, 0.0f, 3.0f);
	}

	shaders.push_back(&fireflyShaderProgram);

	fireflyShaderProgram.CompileSource(
		"shaders/666_main_scene/billboard_particle.vert",
		"shaders/666_main_scene/full_transparent.frag"); // quad
	blurShader.CompileSource("shaders/666_main_scene/hdr.vert", "shaders/666_main_scene/blur.frag");

	fireflyTexture = stbCreateTexture("data/sprites/firefly.png", true, true, true);

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
	/////////////////////////////			PARTICULES			//////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////
	//particle_position.resize(MaxParticles);



	glGenBuffers(2, &VBO[0]);
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

	//bind particle position data
	glEnableVertexAttribArray(2);
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(float), NULL, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);

	//unbind Vertex Array
	glBindVertexArray(0);

	glm::vec3 botRightLimit = glm::vec3(-10, -10, -10);
	glm::vec3 topLeftLimit = glm::vec3(10, 10, 10);
	glm::vec3 range = botRightLimit - topLeftLimit;
	//Init particles
	for (int i = 0; i < MaxParticles; i++) {
		ParticlesContainer[i].topLeftLimit = botRightLimit;
		ParticlesContainer[i].bottomRightLimit = topLeftLimit;

		ParticlesContainer[i].pos = glm::vec3(
			rand() % 20 + botRightLimit.x,
			rand() % 20 + botRightLimit.y,
			rand() % 20 + botRightLimit.z
		);

		LastUsedParticle = i;
	}
}

void FireflyDrawingProgram::Draw()
{
	glEnable(GL_DEPTH_TEST);
	Engine* engine = Engine::GetPtr();
	Configuration& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();
	float dt = engine->GetDeltaTime();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	fireflyShaderProgram.Bind();

	// Bind our texture in Texture Unit 0
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, fireflyTexture);
	glBindVertexArray(VAO);

	const int ParticleCount = ProcessParticles(dt);

	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / config.screenHeight, 0.1f, 100.0f);

	// Same as the billboards tutorial
	fireflyShaderProgram.SetVec3("CameraRight", viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	fireflyShaderProgram.SetVec3("CameraUp", viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
	fireflyShaderProgram.SetMat4("VP", projection * viewMatrix);

	//Update the buffer with all the positions
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	//glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticleCount * 3 * sizeof(float), &particle_position);

	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
	// but faster.
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ParticleCount);

	glBindVertexArray(0);

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
}

void FireflyDrawingProgram::Destroy()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glDeleteBuffers(1, &particles_position_buffer);

	glDeleteTextures(1, &fireflyTexture);

	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(2, &EBO);
}

int FireflyDrawingProgram::ProcessParticles(float dt)
{
	Engine* engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();


	// Simulate all particles
	int ParticlesCount = 0;
	for (int i = 0; i < MaxParticles; i++) {
		FireflyParticle& p = ParticlesContainer[i]; // shortcut

		// Simulate simple physics : gravity only, no collisions
		//p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * dt * 0.5f;
		//p.pos += p.speed * dt;
		//p.cameradistance = glm::length(p.pos - camera.Position);
		//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;
		p.pos += glm::vec3(0.0f, 5.0f, 0.0f) * dt;
		// Fill the GPU buffer
		particle_position[3 * i + 0] = p.pos.x;
		particle_position[3 * i + 1] = p.pos.y;
		particle_position[3 * i + 2] = p.pos.z;

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
	config.bgColor = { 0,0,0,1 };

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