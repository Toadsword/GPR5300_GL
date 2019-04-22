#include <light.h>

void DirectionLight::Bind(Shader& shader, int index)
{
	shader.SetVec3("directionLight.direction", direction);
	shader.SetFloat("directionLight.intensity", intensity);
}

void PointLight::Bind(Shader& shader, int index)
{
	std::string i = std::to_string(index);

	shader.SetVec3("pointLights[" + i + "].position", position);
	shader.SetFloat("pointLights[" + i + "].constant", constant);
	shader.SetFloat("pointLights[" + i + "].linear", linear);
	shader.SetFloat("pointLights[" + i + "].quadratic", quadratic);
	shader.SetFloat("pointLights[" + i + "].intensity", intensity);
}

void SpotLight::Bind(Shader& shader, int index)
{
	std::string i = std::to_string(index);
	shader.SetVec3("spotLights[" + i + "].position", position);
	shader.SetVec3("spotLights[" + i + "].direction", direction);
	shader.SetFloat("spotLights[" + i + "].cutOff", cutOff);
	shader.SetFloat("spotLights[" + i + "].outerCutOff", outerCutOff);
	shader.SetFloat("spotLights[" + i + "].intensity", intensity);
}
