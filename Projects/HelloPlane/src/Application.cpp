#include "Application.h"
#include <vector>
#include "ShaderFuncs.h"
#include <chrono>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"


void Application::setupShaders()
{
	std::string vertexShader = loadTextFile("shaders/passthru.vert");
	std::string fragmentShader = loadTextFile("shaders/passthru.frag");
	programs["passthru"] = InitializeProgram(vertexShader, fragmentShader);

	//Obtemenos la localidad de la variable en los shaders y
	// la guardamos en c++ en nuestro mapa
	uniforms["time"] = glGetUniformLocation(programs["passthru"], "time");
	uniforms["camera"] = glGetUniformLocation(programs["passthru"], "camera");
	uniforms["modelTrans"] = glGetUniformLocation(programs["passthru"], "modelTrans");
	uniforms["projction"] = glGetUniformLocation(programs["passthru"], "projection");
}

void Application::setup()
{
	//Crear Plano
	plane.createPlane(100);
	plane.cleanMemory();
	geometry["plane"] = plane.vao;	//Cargar shaders, compilarlos y ligarlos
	setupShaders();
}

void Application::update()
{
	// Convertir a milisegundos en punto flotante
	auto ahora = std::chrono::system_clock::now();
	// 2. Extraer los milisegundos desde epoch (1 de enero de 1970) como float
	time = std::chrono::duration<float, std::milli>(ahora.time_since_epoch()).count();
	
	modelTrans = glm::mat4(1.0f);

	glm::vec3 eye = glm::vec3(0.0f, 2.5f, 2.5f);
	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.f, 0.0f);

	camera = glm::lookAt(eye, center, up);

	projection = glm::perspective(glm::radians(45.0f), 1024.0f/104.0f, 0.1f, 100.0f);
}

void Application::draw() 
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_FRONT, GL_LINE);
	//Seleccionar programa (shaders)
	glUseProgram(programs["passthru"]);

	//doy valores a las uniform
	glUniform1f(uniforms["time"], time);
	glUniformMatrix4fv(uniforms["camera"], 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(uniforms["modelTrans"], 1, GL_FALSE, glm::value_ptr(modelTrans));
	glUniformMatrix4fv(uniforms["projection"], 1, GL_FALSE, glm::value_ptr(projection));

	//Seleccionar la geometria (el triangulo)
	glBindVertexArray(geometry["plane"]);

	//glDraw()
	glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());
}

Application::~Application() 
{}