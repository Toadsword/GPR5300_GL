#pragma once

#include <functional>
#include <vector>
#include <SFML/Window.hpp>
#include "SFML/Graphics/RenderTarget.hpp"
#include <chrono>
#include "SFML/Graphics/RenderWindow.hpp"

class DrawingProgram;

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
	
	void AddDrawingProgram(DrawingProgram* drawingProgram);


	static Engine* GetPtr();
private:
	void SwitchWireframeMode();
	static Engine* enginePtr;
	sf::RenderWindow* window = nullptr;
	std::vector<DrawingProgram*> drawingPrograms;
	

	sf::Clock deltaClock;
	sf::Clock engineClock;
	sf::Time dt;
	int selectedDrawingProgram = -1;
	bool wireframeMode = false;
	bool debugInfo = true;
	bool drawingProgramsHierarchy = true;
	bool inspector = true;
};

