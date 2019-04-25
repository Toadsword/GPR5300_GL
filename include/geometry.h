#pragma once
#include <vector>

class Plane
{
public:
	void Init();
	void Draw();
	unsigned GetVAO();
private:
	std::vector<float> vertices;
	unsigned quadVAO;
	unsigned quadVBO;
};
