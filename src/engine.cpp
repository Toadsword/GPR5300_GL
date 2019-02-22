
#include <iostream>
#include <engine.h>
#include<graphics.h>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include "imgui-SFML.h"

Engine::~Engine()
{
	for(auto* drawingProgram : drawingPrograms)
	{
		delete drawingProgram;
	}
	drawingPrograms.clear();
}

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
	for (auto drawingProgram : drawingPrograms)
	{
		drawingProgram->Init();
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
			else if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::Key::Num1)
				{
					SwitchWireframeMode();
				}
			}
		}

		// effacement les tampons de couleur/profondeur
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto drawingProgram : drawingPrograms)
		{
			drawingProgram->Draw();
		}

		// termine la trame courante (en interne, échange les deux tampons de rendu)
		window->display();
	}
	delete window;

}

void Engine::AddDrawingProgram(DrawingProgram* hello_triangle_drawing_program)
{
	drawingPrograms.push_back(hello_triangle_drawing_program);
}


void Engine::SwitchWireframeMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, wireframeMode?GL_FILL:GL_LINE);
	wireframeMode = !wireframeMode;
	
}

