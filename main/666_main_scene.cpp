
#include <algorithm>

#include <engine.h>
#include <graphics.h>
#include <geometry.h>

#include <imgui.h>
#include <Remotery.h>


#define Terrain
#ifdef Terrain

class TerrainDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
	void UpdateUi() override;
private:
	Shader shaderProgram;

	float lightPos[3] = { 0,0,-9.5 };

	unsigned VBO[2] = {};
	unsigned int VAO = 0;
	unsigned int EBO = 0;

	unsigned terrainTexture = 0;
	unsigned terrainHeightMap = 0;
	unsigned terrainNormalMap = 0;

	float* vertices = nullptr;
	float* texCoords = nullptr;
	unsigned int* indices = nullptr;

	float terrainOriginY = -1.0f;
	float terrainElevationFactor = 5.0f;

	const size_t terrainWidth = 512l;
	const size_t terrainHeight = 512l;
	const float terrainResolution = 0.2f;

	const size_t verticesCount = terrainWidth * terrainHeight;
	const size_t faceCount = 2 * (terrainWidth - 1) * (terrainHeight - 1);

};

void TerrainDrawingProgram::Init()
{
	programName = "Terrain";

	vertices = (float*)calloc(3 * verticesCount, sizeof(float));//vec3, so 3 floats
	texCoords = (float*)calloc(2 * verticesCount, sizeof(float));//vec2, so 2 floats

	for (size_t i = 0l; i < verticesCount; i++)
	{
		vertices[3 * i] = -(float)terrainWidth * terrainResolution / 2.0f + (float)(i % terrainWidth) * terrainResolution;//x
		vertices[3 * i + 1] = 0.0f;//y
		vertices[3 * i + 2] = -(float)terrainHeight * terrainResolution / 2.0f + (float)(i / terrainWidth) * terrainResolution;//z
	}
	for (size_t i = 0l; i < verticesCount; i++)
	{
		const float width = terrainWidth;
		const float height = terrainHeight;
		texCoords[2 * i] = (float)((i % terrainWidth) + 1) / (width + 1);
		texCoords[2 * i + 1] = (float)((i / terrainWidth) + 1) / (height + 1);
	}

	indices = (unsigned *)calloc(3l * faceCount, sizeof(unsigned));
	size_t quad = 0;
	for (size_t y = 0; y < terrainHeight - 1; y++)
	{
		for (size_t x = 0; x < terrainWidth - 1; x++)
		{
			const unsigned origin = x + y * terrainWidth;
			const unsigned originBottom = origin + terrainWidth;

			//face1
			indices[6 * quad] = origin;
			indices[6 * quad + 1] = origin + 1;
			indices[6 * quad + 2] = originBottom;

			//face2
			indices[6 * quad + 3] = origin + 1;
			indices[6 * quad + 4] = originBottom + 1;
			indices[6 * quad + 5] = originBottom;

			quad++;
		}
	}

	shaderProgram.CompileSource("shaders/666_main_scene/terrain.vert", "shaders/666_main_scene/terrain.frag");
	shaders.push_back(&shaderProgram);

	terrainHeightMap = stbCreateTexture("data/terrain/plains/HeightMap.png", true, false);
	terrainTexture = stbCreateTexture("data/terrain/plains/texture.png", true, false);
	terrainNormalMap = stbCreateTexture("data/terrain/plains/NormalMap.png", true, false);

	glGenBuffers(2, &VBO[0]);
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO); //like: new VAO()
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);//Now use our VAO
	//bind vertices data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//bind texture coords data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	//bind vertices index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * sizeof(unsigned), indices, GL_STATIC_DRAW);
	//unbind Vertex Array
	glBindVertexArray(0);
}

void TerrainDrawingProgram::Draw()
{
	Engine* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	rmt_BeginOpenGLSample(HelloTerrainDraw);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / config.screenHeight, 0.1f, 1000.0f);

	shaderProgram.Bind();
	shaderProgram.SetVec3("lightPos", lightPos);
	shaderProgram.SetVec3("viewPos", camera.Position);

	shaderProgram.SetMat4("view", view);
	shaderProgram.SetMat4("projection", projection);
	shaderProgram.SetMat4("model", model);
	shaderProgram.SetFloat("heightResolution", terrainElevationFactor);
	shaderProgram.SetFloat("heightOrigin", terrainOriginY);

	shaderProgram.SetInt("heightMap", 0);
	shaderProgram.SetInt("diffuseMap", 0);
	shaderProgram.SetInt("normalMap", 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainHeightMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrainTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, terrainNormalMap);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	rmt_EndOpenGLSample();
}

void TerrainDrawingProgram::Destroy()
{
	free(vertices);
	free(texCoords);
	free(indices);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(1, &EBO);
}

void TerrainDrawingProgram::UpdateUi()
{
	DrawingProgram::UpdateUi();
	ImGui::Separator();
	ImGui::SliderFloat("Terrain Height Mult", &terrainElevationFactor, -10.0f, 10.0f, "height = %.3f");
	ImGui::SliderFloat("Terrain Height Origin", &terrainOriginY, -10.0f, 10.0f, "height = %.3f");
}
#endif

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
	glm::vec3 startPos, destPos, diffPos, color;
	float timeSinceBegin, timeToEnd, cameraDistance; // *Squared* distance to the camera. if dead : -1.0f
	float scale, glowScale, glowSpeed;
	bool operator<(const FireflyParticle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameraDistance > that.cameraDistance;
	}
};

const int MaxParticles = 100000;

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
	Shader fireflyShaderProgram;
	Shader blurShader;
	Shader hdrShader;

	//Consts (Editable by Imgui)
	int NumFireflies = MaxParticles;
	GLfloat botRightLimit[3] = { -10.0f, -10.0f, -10.0f };
	int range[3] = { 20, 20, 20 };

	Plane hdrPlane;
	GLuint hdrFBO = 0;
	GLuint rboDepth = 0;
	GLuint hdrColorBuffer[2];

	GLuint pingpongFBO[2];
	GLuint pingpongBuffer[2];

	GLuint fireflyTexture;

	GLuint VAO;
	GLuint VBO[2];
	GLuint EBO;
	//Billboard
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
	int LastUsedParticle = 0; //Used to know how much fireflies are empty
	GLfloat exposure = 1.6f;
	
	GLfloat particlesPosition[3 * MaxParticles];
	GLfloat particlesColor[3 * MaxParticles];

	// 1 for scale (for the 3 axis)
	// 1 for glowing effect
	// 1 for glower speed
	GLfloat particlesScale[3 * MaxParticles];

	GLuint particlesPositionBuffer;
	GLuint particlesColorBuffer;
	GLuint particlesScaleBuffer;
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
		ParticlesContainer[i].cameraDistance = -1.0f;

		LastUsedParticle = i;

		// Buffering init values for each particle
		//float exposure = rand() % 64;
		ParticlesContainer[i].color.r = (rand() % 32) / 256.0f;
		ParticlesContainer[i].color.g = (rand() % 78) / 256.0f;
		ParticlesContainer[i].color.b = (rand() % 32) / 256.0f;

		ParticlesContainer[i].scale = (rand() % 4) / 5.0f + 0.8f;
		ParticlesContainer[i].glowScale = std::max((rand() % 10) / 10.0f - 0.3f, 0.2f) / 3.0f;
		ParticlesContainer[i].glowSpeed = (rand() % 6 + 4.0f) / 2.0f;
	}

	fireflyShaderProgram.CompileSource("shaders/666_main_scene/firefly.vert", "shaders/666_main_scene/firefly.frag"); // quad
	shaders.push_back(&fireflyShaderProgram);

	hdrShader.CompileSource("shaders/666_main_scene/hdr.vert", "shaders/666_main_scene/hdr.frag");
	shaders.push_back(&hdrShader);
	
	blurShader.CompileSource("shaders/666_main_scene/hdr.vert", "shaders/666_main_scene/blur.frag");

	fireflyTexture = stbCreateTexture("data/sprites/firefly6.png", true, true, true);

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
			GL_TEXTURE_2D, 0, GL_RGBA16F, config.screenWidth, config.screenHeight, 0, GL_RGBA, GL_FLOAT, NULL
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
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &particlesColorBuffer);
	glGenBuffers(1, &particlesScaleBuffer);
	glGenBuffers(1, &particlesPositionBuffer);

	glGenVertexArrays(1, &VAO); //like: new VAO()
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);//Now use our VAO
	//bind vertices data
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	//bind texture coords data
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

	//bind vertices index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//bind particle Color
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particlesColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(GLfloat), particlesColor, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0); // Size of Color

	//bind particle Scale
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, particlesScaleBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(GLfloat), particlesScale, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0); // Size of Scale

	//bind particle position data
	glEnableVertexAttribArray(4);
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

	//Get matrices
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / (float)config.screenHeight, 0.1f, 100.0f);


	//Bind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	//Clear color OF THE BUFFER
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Use our shader
	fireflyShaderProgram.Bind();

	//bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fireflyTexture);
	glBindVertexArray(VAO);

	//Process particles
	const int ParticleCount = ProcessParticles(dt);

	// Pass billboard reuqired informations to the shader
	fireflyShaderProgram.SetVec3("CameraRight", viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	fireflyShaderProgram.SetVec3("CameraUp", viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
	fireflyShaderProgram.SetMat4("VP", projection * viewMatrix);
	fireflyShaderProgram.SetFloat("time", engine->GetTimeSinceInit());

	// Update the buffer with all the positions
	glBindBuffer(GL_ARRAY_BUFFER, particlesPositionBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticleCount * 3 * sizeof(float), &particlesPosition);

	// Update the buffer with all the Colors
	glBindBuffer(GL_ARRAY_BUFFER, particlesColorBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticleCount * 3 * sizeof(float), &particlesColor);

	// Update the buffer with all the Scale
	glBindBuffer(GL_ARRAY_BUFFER, particlesScaleBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticleCount * 3 * sizeof(float), &particlesScale);

	// Draw the particles
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ParticleCount);

	//glDepthMask(GL_FALSE); // Don't write to depth buffer

	// Post processing pass
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

	glDisable(GL_BLEND);
	glBindVertexArray(0);
}

void FireflyDrawingProgram::Destroy()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	// Delete texture
	glDeleteTextures(1, &fireflyTexture);

	//Delete vertex Arrays
	glDeleteVertexArrays(1, &VAO);

	// Delete Buffers
	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(1, &EBO);

	glDeleteBuffers(1, &particlesPositionBuffer);
	glDeleteBuffers(1, &particlesColorBuffer);
	glDeleteBuffers(1, &particlesScaleBuffer);

	//Delete Render buffers
	glDeleteRenderbuffers(1, &rboDepth);

	//Delete frame buffers
	glDeleteFramebuffers(1, &hdrFBO);
	glDeleteFramebuffers(2, pingpongFBO);
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
	auto* engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();

	// Simulate all particles
	int ParticlesCount = 0;

	//For each particles
	for (int i = 0; i < NumFireflies; i++) 
	{
		FireflyParticle& p = ParticlesContainer[i];

		// Decrease traveltime
		p.timeSinceBegin -= dt;
		
		// if firefly arrived to its destination
		if(p.timeSinceBegin < 0.0f)
		{
			//Find a new location to go !
			p.startPos = p.destPos;
			p.destPos = glm::vec3(
				rand() % range[0] + botRightLimit[0],
				rand() % range[1] + botRightLimit[1],
				rand() % range[2] + botRightLimit[2]
			);
			p.diffPos = p.destPos - p.startPos;

			//Calculate its speed to destination
			p.timeToEnd = glm::length(p.diffPos) / (rand() % 4 * 1.0f + 1);
			p.timeSinceBegin = p.timeToEnd;
		}

		// Parametric blend, ease in and out. Results in this : https://www.wolframalpha.com/input/?i=t%5E2(3-2t)
		// Go from 0 to 1. (due to 1 - ...)
		float t = 1 - p.timeSinceBegin / p.timeToEnd;
		float change = pow(t, 2) / (2.0f * (pow(t, 2) - t) + 1.0f);


		glm::vec3 pNewPos = p.startPos + p.diffPos * change;
		p.cameraDistance = glm::length(pNewPos - camera.Position);
		// Calculate position and fill GPU budder
		particlesPosition[3 * i + 0] = pNewPos.x;
		particlesPosition[3 * i + 1] = pNewPos.y;
		particlesPosition[3 * i + 2] = pNewPos.z;

		particlesColor[3 * i + 0] = p.color.r;
		particlesColor[3 * i + 1] = p.color.g;
		particlesColor[3 * i + 2] = p.color.b;

		particlesScale[3 * i + 0] = p.scale;
		particlesScale[3 * i + 1] = p.glowScale;
		particlesScale[3 * i + 2] = p.glowSpeed;

		ParticlesCount++;
	}

	// Order the particles
	SortParticles();

	return ParticlesCount;
}

void FireflyDrawingProgram::SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles - 1]);
}
#endif

#define Skybox
#ifdef Skybox
class SkyboxDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;

private:
	GLuint cubemapTexture;
	Shader cubemapShader;
	GLuint cubeMapVAO;
	GLuint cubeMapVBO;

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
};

void SkyboxDrawingProgram::Init()
{
	programName = "Skybox";

	cubemapShader.CompileSource(
		"shaders/666_main_scene/skybox.vert",
		"shaders/666_main_scene/skybox.frag"
	);
	shaders.push_back(&cubemapShader);
	/*std::vector<std::string> faces =
	{
		"data/skybox/nebula/purplenebula_lf.tga",
		"data/skybox/nebula/purplenebula_rt.tga",
		"data/skybox/nebula/purplenebula_up.tga",
		"data/skybox/nebula/purplenebula_dn.tga",
		"data/skybox/nebula/purplenebula_ft.tga",
		"data/skybox/nebula/purplenebula_bk.tga"
	};*/
	/*std::vector<std::string> faces =
	{
		"data/skybox/emerald/emeraldfog_lf.tga",
		"data/skybox/emerald/emeraldfog_rt.tga",
		"data/skybox/emerald/emeraldfog_up.tga",
		"data/skybox/emerald/emeraldfog_dn.tga",
		"data/skybox/emerald/emeraldfog_ft.tga",
		"data/skybox/emerald/emeraldfog_bk.tga"
	};*/
	std::vector<std::string> faces =
	{
		"data/skybox/fluffballday/FluffballDayLeft.hdr",
		"data/skybox/fluffballday/FluffballDayRight.hdr",
		"data/skybox/fluffballday/FluffballDayTop.hdr",
		"data/skybox/fluffballday/FluffballDayBottom.hdr",
		"data/skybox/fluffballday/FluffballDayFront.hdr",
		"data/skybox/fluffballday/FluffballDayBack.hdr"
	};
	cubemapTexture = LoadCubemap(faces);

	glGenVertexArrays(1, &cubeMapVAO);
	glGenBuffers(1, &cubeMapVBO);

	glBindVertexArray(cubeMapVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeMapVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void SkyboxDrawingProgram::Draw()
{
	auto* engine = Engine::GetPtr();
	auto& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();

	const glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),(float)config.screenWidth / (float)config.screenHeight,0.1f,100.0f);

	glDepthFunc(GL_LEQUAL);
	const glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	cubemapShader.Bind();
	cubemapShader.SetMat4("projection", projection);
	cubemapShader.SetMat4("view", skyboxView);
	glBindVertexArray(cubeMapVAO);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void SkyboxDrawingProgram::Destroy()
{
	
}
#endif

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1920;
	config.screenHeight = 1024;
	config.windowName = "Main Scene";
	config.bgColor = { 1,1,1,1 };

#ifdef Camera
	engine.AddDrawingProgram(new CameraProgram());
#endif

#ifdef Skybox
	engine.AddDrawingProgram(new SkyboxDrawingProgram());
#endif

#ifdef Terrain
	engine.AddDrawingProgram(new TerrainDrawingProgram());
#endif

#ifdef Firefly
	engine.AddDrawingProgram(new FireflyDrawingProgram());
#endif

	engine.Init();

	engine.GameLoop();
	return EXIT_SUCCESS;
}