#pragma once

#include <vector>
#include "vector.h"

#ifdef USE_SFML2
#include <SFML/Window/Keyboard.hpp>
#include "SFML/System/Vector2.hpp"
#endif

#ifdef USE_SDL2
#include <SDL.h>
#endif

class InputManager
{
public:
	explicit InputManager();
#ifdef USE_SFML2
	bool GetButtonDown(sf::Keyboard::Key key);
	bool GetButtonUp(sf::Keyboard::Key key);
	bool GetButton(sf::Keyboard::Key key);
#endif
#ifdef USE_SDL2
	
	bool GetButtonDown(SDL_Keycode key);
	bool GetButtonUp(SDL_Keycode key);
	bool GetButton(SDL_Keycode key);
#endif

	Vec2f GetMousePosition();
	float GetMouseWheelDelta();
private:
	friend class Engine;
	void Update();
	std::vector<char> previousButtonStatus;
	float wheelDelta;
};
