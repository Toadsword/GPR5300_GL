#pragma once

#include <functional>
#include <vector>
#include <SFML/Window.hpp>
#include "SFML/Graphics/RenderTarget.hpp"
#include <chrono>
#include "SFML/Graphics/RenderWindow.hpp"
#include "input.h"

class DrawingProgram;

struct Configuration
{
	unsigned int screenWidth = 800;
	unsigned int screenHeight = 600;

	sf::Color bgColor = sf::Color::Black;

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
	sf::RenderWindow* GetWindow();
	void AddDrawingProgram(DrawingProgram* drawingProgram);


	static Engine* GetPtr();
private:
	void SwitchWireframeMode();
	static Engine* enginePtr;
	sf::RenderWindow* window = nullptr;
	std::vector<DrawingProgram*> drawingPrograms;
	InputManager inputManager;
	Configuration configuration;
	sf::Clock deltaClock;
	sf::Clock engineClock;
	sf::Time dt;
	int selectedDrawingProgram = -1;
	bool wireframeMode = false;
	bool debugInfo = true;
	bool drawingProgramsHierarchy = true;
	bool inspector = true;
};

