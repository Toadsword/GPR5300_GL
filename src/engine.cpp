
#include <iostream>
#include <engine.h>

#include "GL/glew.h"
#include <SFML/OpenGL.hpp>

void Engine::Init()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 5;

	window = new sf::Window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
	window->setVerticalSyncEnabled(true);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
	}
	for (auto func : initFunctions)
	{
		func();
	}
}

void Engine::GameLoop()
{
	bool running = true;
	while (running)
	{
		// gestion des évènements
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				// on stoppe le programme
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				// on ajuste le viewport lorsque la fenêtre est redimensionnée
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}

		// effacement les tampons de couleur/profondeur
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(auto func : drawingFunctions)
		{
			func();
		}

		// termine la trame courante (en interne, échange les deux tampons de rendu)
		window->display();
	}
	delete window;

}

void Engine::AddInitFunction(std::function<void(void)> func)
{
	initFunctions.push_back(func);
}

void Engine::AddDrawingFunction(std::function<void(void)> func)
{
	drawingFunctions.push_back(func);
}

