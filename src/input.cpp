#include <input.h>
#include <engine.h>

#ifdef USE_SFML2
InputManager::InputManager()
{
	previousButtonStatus.resize(sf::Keyboard::Key::KeyCount);
}

bool InputManager::GetButtonDown(sf::Keyboard::Key key)
{
	return !previousButtonStatus[key] && sf::Keyboard::isKeyPressed(key);
}

bool InputManager::GetButtonUp(sf::Keyboard::Key key)
{
	return previousButtonStatus[key] && !sf::Keyboard::isKeyPressed(key);
}

bool InputManager::GetButton(sf::Keyboard::Key key)
{
	return sf::Keyboard::isKeyPressed(key);
}

Vec2f InputManager::GetMousePosition()
{
	Engine* engine = Engine::GetPtr();
	auto* window = engine->GetWindow();
	return static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window));
}

void InputManager::Update()
{
	for (int i = 0; i < sf::Keyboard::Key::KeyCount; i++)
	{
		previousButtonStatus[i] = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i));
	}
}
#endif
#ifdef USE_SDL2
InputManager::InputManager()
{
}
bool InputManager::GetButtonDown(SDL_Keycode key)
{
	return false;
}
bool InputManager::GetButtonUp(SDL_Keycode key)
{
	return false;
}
bool InputManager::GetButton(SDL_Keycode key)
{
	return false;
}
#endif
float InputManager::GetMouseWheelDelta()
{
	return wheelDelta;
}

