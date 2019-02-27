#include <engine.h>
#include <graphics.h>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>


class HelloCameraDrawingProgram : public DrawingProgram
{
public:
    void Init() override;
    void Draw() override;
private:
    Shader shaderProgram;
};

void HelloCameraDrawingProgram::Init()
{
    shaderProgram.Init("shaders/hello_camera/camera_vertex.glsl", "shaders/hello_camera/camera_fragment.glsl");
}

void HelloCameraDrawingProgram::Draw()
{

}


int main()
{
    Engine engine;

    auto& config = engine.GetConfiguration();
    config.screenSizeX = 1024;
    config.screenSizeY = 1024;
    config.bgColor = sf::Color::Black;
    config.windowName = "Hello Camera";

    engine.AddDrawingProgram(new HelloCameraDrawingProgram());

    engine.Init();
    engine.GameLoop();

    return EXIT_SUCCESS;
}