#include <GLFW/glfw3.h>
#include "Application.h"

Application app;
//void myFirstCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//    {
//        glfwSetWindowShouldClose(window, 1);
//    }
//       
//}

int main(void)
{
	app.setup();

	while (!glfwWindowShouldClose(app.getWindow()))
	{
		app.update();
		app.draw();
	}

	return 0;
}