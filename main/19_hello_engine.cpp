#include <engine.h>
#include <graphics.h>
#include <scene.h>

int main(int argc, char** argv)
{
	Engine engine;

	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Engine";
	auto* sceneDrawingProgram = new SceneDrawingProgram();
	auto & scene = sceneDrawingProgram->GetScene();
	scene.SetScenePath("scenes/19_engine_scene.json");
	engine.AddDrawingProgram(sceneDrawingProgram);

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}