/*
 MIT License

 Copyright (c) 2017 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <iostream>
#include <engine.h>
#include <graphics.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

class TestProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
	void UpdateUi() override;
private:
	void ProcessInput();

	static const int cubeLength = 10;

	//pos + normal + texture coords
	float vertices[8 * 36] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glm::vec3 cubePositions[cubeLength] =
	{
		glm::vec3(-20.0f,  20.0f, -10.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	float cubeInitialAngle[cubeLength] =
	{
		M_PI / 2.0f,
		M_PI / 4.0f,
		-M_PI / 4.0f,
		-M_PI / 2.0f,
		M_PI * 3.0f / 2.0f,
		M_PI * 3.0f / 4.0f,
		M_PI * 3.0f / 4.0f,
		M_PI / 3.0f,
		M_PI / 6.0f,
		-M_PI / 6.0f
	};

	unsigned int VBO = 0, cubeVAO = 0;
	unsigned int lightVAO;

	glm::vec3 lightPos = { 2.0f, 0.0f, 2.0f };

	Shader shaderProgram;
	Shader lampProgram;

	//GLuint textureStar;

	float objectColor[3] = { 1.0f, 0.5f, 0.31f };
	float lightColor[3] = { 1.0f,1.0f,1.0f };
	/*
	BasicMaterial material =
	{
		glm::vec3(1.0f, 0.5f, 0.31f),
		glm::vec3(1.0f, 0.5f, 0.31f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		32.0f
	};
	*/
	int diffuseMapTexture = 0;
	int specularMapTexture = 0;
};

void TestProgram::Init()
{
	programName = "BabaIsNotYou";


	shaderProgram.CompileSource(
		"shaders/__01_redo_to_5/material.vert",
		"shaders/__01_redo_to_5/material.frag"
	);
	lampProgram.CompileSource(
		"shaders/__01_redo_to_5/lamp.vert",
		"shaders/__01_redo_to_5/lamp.frag"
	);
	shaders.push_back(&shaderProgram);
	shaders.push_back(&lampProgram);

	//textureStar = stbCreateTexture("data/sprites/star.png");
	diffuseMapTexture = stbCreateTexture("data/sprites/container2.png");
	specularMapTexture = stbCreateTexture("data/sprites/container2_specular.png");

	glGenVertexArrays(1, &cubeVAO); // like new VAO()
	glGenBuffers(1, &VBO); //like new VBO()

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture coords attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	//reusing the cube data, without the normals
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void TestProgram::Draw()
{
	ProcessInput();

	Engine* engine = Engine::GetPtr();
	Configuration& config = engine->GetConfiguration();
	Camera& camera = engine->GetCamera();

	//Camera config
	glm::mat4 view = engine->GetCamera().GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)config.screenWidth / config.screenHeight, 0.1f, 100.0f);

	// Calculating light position
	lightPos = glm::vec3(5.0f*sin(2.0f*M_PI / 8.0f*engine->GetTimeSinceInit()), lightPos.y, 2.0f*cos(2.0f*M_PI / 8.0f*engine->GetTimeSinceInit()));

	glEnable(GL_DEPTH_TEST);

	//ShaderProgram (Cubes)
	shaderProgram.Bind();
	//glBindTexture(GL_TEXTURE_2D, textureStar);
	
	glUniform3fv(glGetUniformLocation(shaderProgram.GetProgram(), "viewPos"), 1, &camera.Position[0]);
	glUniform3fv(glGetUniformLocation(shaderProgram.GetProgram(), "objectColor"), 1, objectColor);
	//material parameters
	glUniform1i(glGetUniformLocation(shaderProgram.GetProgram(), "material.diffuse"), 0);//TEXTURE0
	glUniform1i(glGetUniformLocation(shaderProgram.GetProgram(), "material.specular"), 1);//TEXTURE1
	glUniform1f(glGetUniformLocation(shaderProgram.GetProgram(), "material.shininess"), 64.0f);

	//Light parameter
	glUniform3fv(glGetUniformLocation(shaderProgram.GetProgram(), "light.position"), 1, &lightPos[0]);
	glUniform3f(glGetUniformLocation(shaderProgram.GetProgram(), "light.ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(shaderProgram.GetProgram(), "light.diffuse"), 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
	glUniform3f(glGetUniformLocation(shaderProgram.GetProgram(), "light.specular"), 1.0f, 1.0f, 1.0f);

	//Calculating View & projection
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.GetProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.GetProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMapTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMapTexture);

	glBindVertexArray(cubeVAO);
	for (int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f); //model transform matrix
		model = glm::translate(model, cubePositions[i]);
		model = glm::rotate(model, cubeInitialAngle[i], glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(engine->GetTimeSinceInit()*45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(engine->GetTimeSinceInit()*45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		int modelLoc = glGetUniformLocation(shaderProgram.GetProgram(), "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		
		glDrawArrays(GL_TRIANGLES, 0, 36); // 36 est le nombre de points pour le cube
	}
	glBindVertexArray(0);

	//Draw lamp
	lampProgram.Bind();
	lampProgram.SetVec3("lightColor", lightColor[0], lightColor[1], lightColor[2]);

	//Calculating View & proj
	glUniformMatrix4fv(glGetUniformLocation(lampProgram.GetProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(lampProgram.GetProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f)); //Make it smaller

	glUniformMatrix4fv(glGetUniformLocation(lampProgram.GetProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void TestProgram::Destroy()
{
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);
}

void TestProgram::ProcessInput()
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

void TestProgram::UpdateUi()
{
	ImGui::Separator();
	ImGui::ColorEdit3("objectColor", objectColor);
	ImGui::ColorEdit3("lightColor", lightColor);
}

int main(int argc, char **argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	//config.bgColor = sf::Color::Black;
	config.windowName = "Hello Texture";
	
	engine.AddDrawingProgram(new TestProgram());
	engine.Init();
	engine.GameLoop();
	return EXIT_SUCCESS;
}
