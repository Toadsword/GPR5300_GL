#include <algorithm>
#include <map>
#include <engine.h>
#include <graphics.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

// CPU representation of a particle
struct Particle {
	glm::vec3 pos, speed;
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};
const int MaxParticles = 100000;


class HelloParticleDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;

	int FindUnusedParticle();
	int ProcessParticles(float dt);
	void SortParticles();

	void ProcessInput();

private:
	Shader windowShaderProgram;
	Shader grassShaderProgram;

	unsigned VAO;
	unsigned grassTexture;

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

	const static int grassNmb = 3;
	glm::vec3 grassPositions[grassNmb] =
	{
		glm::vec3(1., 0.0,-3.0),
		glm::vec3(1., 0.0,-5.0),
		glm::vec3(-2.,0.0,-1.0)
	};

	unsigned int instanceVBO;

	std::vector<Particle> ParticlesContainer; 
	int LastUsedParticle = 0;

	//std::vector<glm::vec3> particule_position;
	float particule_position[3 * MaxParticles];

	unsigned int particles_position_buffer;
};

void HelloParticleDrawingProgram::Init()
{
	programName = "HelloParticle";

	ParticlesContainer.resize(MaxParticles);
	for (int i = 0; i < MaxParticles; i++) {
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
		ParticlesContainer[i].speed = glm::vec3(-1.0f, 0.0f, 3.0f);
	}

	shaders.push_back(&grassShaderProgram);
	
	grassShaderProgram.CompileSource(
		"shaders/52_hello_particle/billboard_particle.vert",
		"shaders/52_hello_particle/full_transparent.frag"); // quad

	grassTexture = stbCreateTexture("data/sprites/grass.png", true, true, true);

	//////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////			PARTICULES			//////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////
	//particule_position.resize(MaxParticles);
	
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

}

void HelloParticleDrawingProgram::Draw()
{
	ProcessInput();

	glEnable(GL_DEPTH_TEST);
	Engine* engine = Engine::GetPtr();
	Configuration& config = engine->GetConfiguration();
	auto& camera = engine->GetCamera();
	float dt = engine->GetDeltaTime();



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	grassShaderProgram.Bind();

	// Bind our texture in Texture Unit 0
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glBindVertexArray(VAO);

	const int ParticleCount = ProcessParticles(dt);

	glm::mat4 viewMatrix = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / config.screenHeight, 0.1f, 100.0f);

	// Same as the billboards tutorial
	grassShaderProgram.SetVec3("CameraRight", viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	grassShaderProgram.SetVec3("CameraUp", viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
	grassShaderProgram.SetMat4("VP", projection * viewMatrix);

	//Update the buffer with all the positions
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	//glBufferData(GL_ARRAY_BUFFER, MaxParticles * 3 * sizeof(float), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticleCount * 3 * sizeof(float), &particule_position);

	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
	// but faster.
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ParticleCount);

	glBindVertexArray(0);
}

void HelloParticleDrawingProgram::Destroy()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glDeleteBuffers(1, &particles_position_buffer);

	glDeleteTextures(1, &grassTexture);

	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(2, &EBO);
}

void HelloParticleDrawingProgram::ProcessInput()
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

int HelloParticleDrawingProgram::ProcessParticles(float dt)
{
	Engine* engine = Engine::GetPtr();
	auto& camera = engine->GetCamera();

	int newparticles = 1;

	for (int i = 0; i < newparticles; i++) {
		int particleIndex = FindUnusedParticle();
		ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
		ParticlesContainer[particleIndex].pos = glm::vec3(0, 0, -4.0f);
		/*
		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
		// Very bad way to generate a random direction; 
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc)
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);

		ParticlesContainer[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;

		*/
		//ParticlesContainer[particleIndex].speed = glm::vec3(0, 0, 3.0f);
	}


	// Simulate all particles
	int ParticlesCount = 0;
	for (int i = 0; i < MaxParticles; i++) {

		Particle& p = ParticlesContainer[i]; // shortcut

		if (p.life > 0.0f) {

			// Decrease life
			p.life -= dt;
			if (p.life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				//p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * dt * 0.5f;
				//p.pos += p.speed * dt;
				//p.cameradistance = glm::length(p.pos - camera.Position);
				//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;
				p.pos += glm::vec3(0.0f, 5.0f, 0.0f) * dt;
				// Fill the GPU buffer
				particule_position[3 * i + 0] = p.pos.x;
				particule_position[3 * i + 1] = p.pos.y;
				particule_position[3 * i + 2] = p.pos.z;
			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}

			ParticlesCount++;
		}
	}

	SortParticles();
	
	return ParticlesCount;
}

void HelloParticleDrawingProgram::SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles - 1]);
}

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int HelloParticleDrawingProgram::FindUnusedParticle() {

	for (int i = LastUsedParticle; i < MaxParticles; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Particle";
	config.bgColor = { 1,1,1 };

	engine.AddDrawingProgram(new HelloParticleDrawingProgram());

	engine.Init();

	engine.GameLoop();
	return EXIT_SUCCESS;
}
