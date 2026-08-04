#pragma once
#include <map>
#include <string>
#include "glad/glad.h"
#include "Plane.h"

class Application
{
public:
	void setup();
	void update();
	void draw();
	~Application();

	std::map<std::string, GLuint> geometry;
	std::map<std::string, GLuint> programs;
	std::map<std::string, GLuint> uniforms;

	float time{ 0.0f };
	Plane plane;

private:
	
	void setupShaders();
};