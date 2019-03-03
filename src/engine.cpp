
#include <iostream>

#include <GL/glew.h>
#include <engine.h>
#include <graphics.h>
#ifdef USE_SFML2
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>
#include "imgui-SFML.h"
#endif
#ifdef USE_EMSCRIPTEN
#include <emscripten.h> 
#endif
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
#ifdef USE_SFML2
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 6;

	window = new sf::RenderWindow(sf::VideoMode(configuration.screenWidth, configuration.screenHeight), configuration.windowName, sf::Style::Default, settings);
	//window->setVerticalSyncEnabled(true);
	ImGui::SFML::Init(*window);
#endif

#ifdef USE_SDL2
	window = SDL_CreateWindow(
		configuration.windowName.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		configuration.screenWidth,
		configuration.screenHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	// Check that everything worked out okay
	if (window == nullptr)
	{
		std::cerr << "Unable to create window\n";
		return;
	}
	glContext = SDL_GL_CreateContext(window);

	// Set our OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	// Turn on double buffering with a 24bit Z buffer.
	// You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#endif
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

#ifdef USE_SFML2
void Engine::GameLoop()
{
	running = true;
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
				std::cout << "Key Pressed: " << event.key.code << "\n";
				if(event.key.code == sf::Keyboard::Key::Num1)
				{
					SwitchWireframeMode();
				}
				if (event.key.code == sf::Keyboard::Key::Num2)
				{
					debugInfo = !debugInfo;
				}
				if(event.key.code == imguiKey)
				{
					enableImGui = !enableImGui;
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
		if (enableImGui)
		{
			ImGui::SFML::Update(*window, dt);
			UpdateUi();
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto drawingProgram : drawingPrograms)
		{
			drawingProgram->Draw();
		}

		//window->pushGLStates();
		if(enableImGui)
			ImGui::SFML::Render(*window);
		//window->popGLStates();

		//switch framebuffer
		window->display();
		inputManager.Update();
	}
	delete window;

}
#endif

#ifdef USE_SDL2

void Engine::Loop()
{
	Engine* engine = Engine::GetPtr();
	auto currentFrame = engine->timer.now();
	engine->dt = std::chrono::duration_cast<ms>(currentFrame - engine->previousFrameTime).count() / 1000.f;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			engine->running = false;
		}
		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				std::cout << "Window Size: " << event.window.data1 << ", " << event.window.data2 << "\n";
				Vec2f newWindowSize = Vec2f(event.window.data1, event.window.data2);
				std::cout << "New Window Size: " << newWindowSize << "\n";
				glViewport(0, 0, newWindowSize.x, newWindowSize.y);
				engine->configuration.screenWidth = event.window.data1;
				engine->configuration.screenHeight = event.window.data2;
			}
		}
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto drawingProgram : engine->drawingPrograms)
	{
		drawingProgram->Draw();
	}
	SDL_GL_SwapWindow(engine->window);
}


void Engine::GameLoop()
{
	running = true;
	engineStartTime = timer.now();
	previousFrameTime = engineStartTime;

#ifdef __EMSCRIPTEN__
	// void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
	emscripten_set_main_loop(Loop, 60,1);
#else
	while (running)
	{
		Loop();
	}
#endif
	// Delete our OpengL context
	SDL_GL_DeleteContext(glContext);

	// Destroy our window
	SDL_DestroyWindow(window);

	// Shutdown SDL 2
	SDL_Quit();
}
#endif
void Engine::UpdateUi()
{
#ifdef USE_SFML2
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
#endif
}

float Engine::GetDeltaTime()
{
#ifdef USE_SFML2
	return dt.asSeconds();
#endif
#ifdef USE_SDL2
	return dt;
#endif
}

float Engine::GetTimeSinceInit()
{
#ifdef USE_SFML2
	return engineClock.getElapsedTime().asSeconds();
#endif
#ifdef USE_SDL2
	return std::chrono::duration_cast<ms>(previousFrameTime - engineStartTime).count() / 1000.f;
#endif
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
#ifdef USE_SFML2
sf::RenderWindow* Engine::GetWindow()
{
	return window;
}
#endif


