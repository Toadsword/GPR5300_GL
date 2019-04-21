#include <engine.h>
#include <graphics.h>
#include <camera.h>
class SceneDrawingProgram;
class HelloWaterDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
private:
	Camera underWaterCamera;
	Camera* sceneCamera = nullptr;
	SceneDrawingProgram* scene = nullptr;
};

class SceneDrawingProgram : public DrawingProgram
{
public:
	void Init() override;
	void Draw() override;
	void Destroy() override;
	Camera* GetCamera();
protected:
	Camera camera;
};


void HelloWaterDrawingProgram::Init()
{
	Engine* engine = Engine::GetPtr();
	auto& drawingPrograms = engine->GetDrawingPrograms();

	scene = dynamic_cast<SceneDrawingProgram*>(drawingPrograms[0]);
	sceneCamera = scene->GetCamera();
}

void HelloWaterDrawingProgram::Draw()
{
	//Invert Camera

	//Stencil pass

	//Draw scene on top of water in reflection map

}

void HelloWaterDrawingProgram::Destroy()
{
}

void SceneDrawingProgram::Init()
{
}

void SceneDrawingProgram::Draw()
{
}

void SceneDrawingProgram::Destroy()
{
}

Camera* SceneDrawingProgram::GetCamera()
{
	return &camera;
}


int main(int argc, char** argv)
{
	Engine engine;
	auto& config = engine.GetConfiguration();
	config.screenWidth = 1024;
	config.screenHeight = 1024;
	config.windowName = "Hello Water";
	engine.AddDrawingProgram(new SceneDrawingProgram());
	engine.AddDrawingProgram(new HelloWaterDrawingProgram());

	engine.Init();
	engine.GameLoop();

	return EXIT_SUCCESS;
}

