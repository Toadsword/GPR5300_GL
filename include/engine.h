#pragma once

#include <functional>
#include <vector>
#include <string>

#ifdef USE_SFML2


#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#endif

#ifdef USE_SDL2

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#ifdef WIN32
#include <SDL_main.h>
#endif
#endif

#include <chrono>
#include <input.h>

class DrawingProgram;

#ifdef USE_SFML2
typedef sf::Color Color;
#endif

#ifdef USE_SDL2
typedef SDL_Color Color;
using ms = std::chrono::duration<float, std::milli>;
#endif

struct Configuration
{
	unsigned int screenWidth = 800;
	unsigned int screenHeight = 600;

	Color bgColor = {0,0,0,0};

	std::string windowName = "OpenGL";
};

class Engine
{
public:
	Engine();
	~Engine();
	void Init();
	void GameLoop();

	void UpdateUi();

	float GetDeltaTime();
	float GetTimeSinceInit();

	Configuration& GetConfiguration();
	InputManager& GetInputManager();
#ifdef USE_SFML2
	sf::RenderWindow* GetWindow();
#endif
	void AddDrawingProgram(DrawingProgram* drawingProgram);


	static Engine* GetPtr();
private:
	void SwitchWireframeMode();
	static Engine* enginePtr;

#ifdef USE_SFML2
	sf::RenderWindow* window = nullptr;
	sf::Clock deltaClock;
	sf::Clock engineClock;
	sf::Time dt;

	const sf::Keyboard::Key imguiKey = static_cast<sf::Keyboard::Key>(52);
#endif

#ifdef USE_SDL2

	static void Loop();
	SDL_Window* window = nullptr;
	SDL_GLContext glContext;
	std::chrono::high_resolution_clock timer;

	std::chrono::time_point<std::chrono::steady_clock> engineStartTime;
	std::chrono::time_point<std::chrono::steady_clock> previousFrameTime;
	float dt;
#endif
	std::vector<DrawingProgram*> drawingPrograms;
	InputManager inputManager;
	Configuration configuration;

	int selectedDrawingProgram = -1;
	bool wireframeMode = false;
	bool debugInfo = true;
	bool drawingProgramsHierarchy = true;
	bool inspector = true;
	bool enableImGui = false;
	bool running = false;
};

