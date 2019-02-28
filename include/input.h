#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include "SFML/System/Vector2.hpp"

class InputManager
{
public:
	explicit InputManager();
	bool GetButtonDown(sf::Keyboard::Key key);
	bool GetButtonUp(sf::Keyboard::Key key);
	bool GetButton(sf::Keyboard::Key key);

	sf::Vector2f GetMousePosition();
	float GetMouseWheelDelta();
private:
	friend class Engine;
	void Update();
	std::vector<char> previousButtonStatus;
	float wheelDelta;
};
