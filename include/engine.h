#pragma once

#include <functional>
#include <vector>
#include <SFML/Window.hpp>

class Engine
{
public:
	void Init();
	void GameLoop();

	void AddInitFunction(std::function<void(void)>);
	void AddDrawingFunction(std::function<void(void)>);
private:
	sf::Window* window = nullptr;
	std::vector< std::function<void(void)>> initFunctions;
	std::vector<std::function<void(void)>> drawingFunctions;
};

