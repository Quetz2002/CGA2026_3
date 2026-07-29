#pragma once
#include <GLFW/glfw3.h>
#include <iostream>


class Application
{
public:
	void setup();
	void update();
	void draw();
	~Application();

	GLFWwindow* getWindow() const { return window; }
private:
	GLFWwindow* window = nullptr;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

};