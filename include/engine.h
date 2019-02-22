#pragma once

#include <functional>
#include <vector>
#include <SFML/Window.hpp>
#include "SFML/Graphics/RenderTarget.hpp"

class Engine
{
public:
	void Init();
	void GameLoop();

	void AddInitFunction(std::function<void(void)>);
	void AddDrawingFunction(std::function<void(void)>);
	
private:
	void SwitchWireframeMode();
	
	sf::Window* window = nullptr;
	std::vector< std::function<void(void)>> initFunctions;
	std::vector<std::function<void(void)>> drawingFunctions;
	bool wireframeMode = false;
};

