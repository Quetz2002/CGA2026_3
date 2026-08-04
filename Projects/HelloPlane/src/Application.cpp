#include "Application.h"
#include <vector>
#include "ShaderFuncs.h"



void Application::setupShaders()
{
	std::string vertexShader = loadTextFile("shaders/passthru.vert");
	std::string fragmentShader = loadTextFile("shaders/passthru.frag");
	programs["passthru"] = InitializeProgram(vertexShader, fragmentShader);

	//Obtenemos la localidad de los chaders y
	//la guardamos en C++ en nuestro mapa
	uniforms["time"] = glGetUniformLocation(programs["passthru"], "time");
}

void Application::setup()
{
	//Crear Plano
	plane.createPlane(1);
	plane.cleanMemory();
	
	//Cargar shaders, compilarlos y ligarlos
	setupShaders();
}

void Application::update()
{
	++time;
}

void Application::draw() 
{
	//Seleccionar programa (shaders)
	glUseProgram(programs["passthru"]);

	//doy valores a las uniform
	glUniform1f(uniforms["time"], time);

	//Seleccionar la geometria (el triangulo)
	glBindVertexArray(geometry["plane"]);

	//glDraw()
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

Application::~Application() 
{}