
#include <iostream>
#include <engine.h>
#include<graphics.h>
#include <GL/glew.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>
#include "imgui-SFML.h"
#include <chrono>
#include "imgui.h"

Engine* Engine::enginePtr = nullptr;

Engine::Engine()
{
	enginePtr = this;
	
}

Engine::~Engine()
{
	for(auto* drawingProgram : drawingPrograms)
	{
		drawingProgram->Destroy();
		delete drawingProgram;
	}
	drawingPrograms.clear();
}

void Engine::Init()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 5;

	window = new sf::RenderWindow(sf::VideoMode(configuration.screenWidth, configuration.screenHeight), configuration.windowName, sf::Style::Default, settings);
	//window->setVerticalSyncEnabled(true);
	ImGui::SFML::Init(*window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
	}
	for (auto drawingProgram : drawingPrograms)
	{
		drawingProgram->Init();
	}
	glClearColor(configuration.bgColor.r, configuration.bgColor.g, configuration.bgColor.b, configuration.bgColor.a);
}

void Engine::GameLoop()
{
	bool running = true;
	engineClock.restart();
	deltaClock.restart();
	while (running)
	{
		dt = deltaClock.restart();

		// Event management
		sf::Event event{};
		while (window->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
			{
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
				configuration.screenWidth = event.size.width;
				configuration.screenHeight = event.size.height;
			}
			else if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::Key::Num1)
				{
					SwitchWireframeMode();
				}
				if (event.key.code == sf::Keyboard::Key::Num2)
				{
					debugInfo = !debugInfo;
				}
			}
			if(event.type == sf::Event::MouseWheelScrolled)
			{
				inputManager.wheelDelta = event.mouseWheelScroll.delta;
			}
			else
			{
				inputManager.wheelDelta = 0.0f;
			}
		}

		ImGui::SFML::Update(*window, dt);
		UpdateUi();
		// effacement les tampons de couleur/profondeur
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto drawingProgram : drawingPrograms)
		{
			drawingProgram->Draw();
		}

		//window->pushGLStates();
		ImGui::SFML::Render(*window);
		//window->popGLStates();

		//switch framebuffer
		window->display();
		inputManager.Update();
	}
	delete window;

}

void Engine::UpdateUi()
{
	const auto windowSize = window->getSize();
	if (debugInfo)
	{
		const auto settings = window->getSettings();

		ImGui::SetNextWindowPos(ImVec2(150, 0), ImGuiCond_Always);
		ImGui::Begin("Debug Info");
		ImGui::Text("OpenGL version: %d.%d", settings.majorVersion, settings.minorVersion);
		ImGui::Text("AA level: %d", settings.antialiasingLevel);
		ImGui::Text("Depth bits: %d", settings.depthBits);
		ImGui::Text("Stencil bits: %d", settings.stencilBits);
		ImGui::Text("FPS: %4.0f", 1.0f / GetDeltaTime());
		ImGui::End();
	}

	if(drawingProgramsHierarchy)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(150.0f, window->getSize().y), ImGuiCond_Always);
		ImGui::Begin("Hierarchy");
		for (int i = 0; i < drawingPrograms.size();i++)
		{
			auto* drawingProgram = drawingPrograms[i];
			if (ImGui::Selectable(drawingProgram->GetProgramName().c_str(), selectedDrawingProgram == i))
			{
				selectedDrawingProgram = i;
			}
		}
		ImGui::End();
	}

	if(inspector)
	{
		ImGui::SetNextWindowPos(ImVec2(windowSize.x - 250.0f, 0), ImGuiCond_FirstUseEver);
		//ImGui::SetNextWindowSize(ImVec2(250.0f, window->getSize().y), ImGuiCond_Always);
		ImGui::Begin("Inspector");
		if(selectedDrawingProgram != -1)
		{
			auto* drawingProgram = drawingPrograms[selectedDrawingProgram];
			auto& shaders = drawingProgram->GetShaders();
			for(int i = 0; i < shaders.size();i++)
			{
				GLint count;

				GLint size; // size of the variable
				GLenum type; // type of the variable (float, vec3 or mat4, etc)

				const GLsizei bufSize = 16; // maximum name length
				GLchar name[bufSize]; // variable name in GLSL
				GLsizei length; // name length

				auto* shader = shaders[i];
				ImGui::Text("Shader %d", i);
				glGetProgramiv(shader->GetProgram(), GL_ACTIVE_ATTRIBUTES, &count);

				for (int j = 0; j < count; j++)
				{
					glGetActiveAttrib(shader->GetProgram(), (GLuint)j, bufSize, &length, &size, &type, name);

					ImGui::Text("Attribute #%d Type: %u Name: %s", j, type, name);
				}

				glGetProgramiv(shader->GetProgram(), GL_ACTIVE_UNIFORMS, &count);

				for (int j = 0; j < count; j++)
				{
					glGetActiveUniform(shader->GetProgram(), (GLuint)j, bufSize, &length, &size, &type, name);

					ImGui::Text("Uniform #%d Type: %u Name: %s", j, type, name);
				}
			}
			drawingProgram->UpdateUi();

		}
		ImGui::End();
	}
}

float Engine::GetDeltaTime()
{
	return dt.asSeconds();
}

float Engine::GetTimeSinceInit()
{
	return engineClock.getElapsedTime().asSeconds();
}

void Engine::AddDrawingProgram(DrawingProgram* drawingProgram)
{
	drawingPrograms.push_back(drawingProgram);
}

Engine* Engine::GetPtr()
{
	return enginePtr;
}


void Engine::SwitchWireframeMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_FILL : GL_LINE);
	wireframeMode = !wireframeMode;
	
}

Configuration &Engine::GetConfiguration()
{
	return configuration;
}

InputManager& Engine::GetInputManager()
{
	return inputManager;
}

sf::RenderWindow* Engine::GetWindow()
{
	return window;
}

