#pragma once

#include <functional>
#include <vector>
#include <SFML/Window.hpp>
#include "SFML/Graphics/RenderTarget.hpp"

class DrawingProgram;

class Engine
{
public:
	~Engine();
	void Init();
	void GameLoop();

	void AddDrawingProgram(DrawingProgram* hello_triangle_drawing_program);

private:
	void SwitchWireframeMode();
	
	sf::Window* window = nullptr;
	std::vector<DrawingProgram*> drawingPrograms;
	bool wireframeMode = false;
};

