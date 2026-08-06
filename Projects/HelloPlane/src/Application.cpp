#include "Application.h"
#include <GLFW/glfw3.h>
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
	uniforms["projection"] = glGetUniformLocation(programs["passthru"], "projection");
	uniforms["color"] = glGetUniformLocation(programs["passthru"], "color");
}

void Application::setup()
{
	//Crear Plano
	plane.createPlane(100);
	plane.cleanMemory();
	geometry["plane"] = plane.vao;	//Cargar shaders, compilarlos y ligarlos
	setupShaders();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(GLFWwindow* window)
{
	double currentTime = glfwGetTime();
	if (lastTime == 0.0)
	{
		lastTime = currentTime;
	}
	float deltaTime = static_cast<float>(currentTime - lastTime);
	lastTime = currentTime;
	time = currentTime * 1000.0; // time in ms for shaders

	if (deltaTime > 0.1f) deltaTime = 0.1f;

	// Obtener la posición del cursor y el tamaño de la ventana
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Calcular offsets normalizados respecto al centro [-1.0, 1.0]
	float ndx = 0.0f;
	float ndy = 0.0f;
	if (width > 0 && height > 0)
	{
		ndx = static_cast<float>((xpos - (width / 2.0)) / (width / 2.0));
		ndy = static_cast<float>((ypos - (height / 2.0)) / (height / 2.0));
	}
	ndx = glm::clamp(ndx, -1.0f, 1.0f);
	ndy = glm::clamp(ndy, -1.0f, 1.0f);

	// Mapear a ángulos de rotación de vuelo:
	// - Pitch (cabeceo alrededor de X): cursor abajo -> pitch arriba (rotación positiva)
	// - Roll (alabeo alrededor de Z): cursor derecha -> roll derecha (rotación negativa)
	// - Yaw (guiñada alreadedor de Y): cursor derecha -> guiñada derecha (rotación negativa)
	float targetRoll = -ndx * glm::radians(45.0f);
	float targetPitch = ndy * glm::radians(30.0f);
	float targetYaw = -ndx * glm::radians(35.0f);

	// Suavizado (inercia)
	float lerpFactor = 5.0f * deltaTime;
	if (lerpFactor > 1.0f) lerpFactor = 1.0f;

	currentRoll = glm::mix(currentRoll, targetRoll, lerpFactor);
	currentPitch = glm::mix(currentPitch, targetPitch, lerpFactor);
	currentYaw = glm::mix(currentYaw, targetYaw, lerpFactor);

	// Construir matrices
	modelTrans = glm::mat4(1.0f);
	modelTrans = glm::rotate(modelTrans, currentYaw, glm::vec3(0.0f, 1.0f, 0.0f));
	modelTrans = glm::rotate(modelTrans, currentPitch, glm::vec3(1.0f, 0.0f, 0.0f));
	modelTrans = glm::rotate(modelTrans, currentRoll, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 eye = glm::vec3(0.0f, 2.5f, 2.5f);
	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	camera = glm::lookAt(eye, center, up);

	float aspect = height > 0 ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
	projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void Application::draw() 
{
	//Seleccionar programa (shaders)
	glUseProgram(programs["passthru"]);

	//doy valores a las uniform
	glUniform1f(uniforms["time"], time);
	glUniformMatrix4fv(uniforms["camera"], 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(uniforms["modelTrans"], 1, GL_FALSE, glm::value_ptr(modelTrans));
	glUniformMatrix4fv(uniforms["projection"], 1, GL_FALSE, glm::value_ptr(projection));

	//Seleccionar la geometria (el triangulo)
	glBindVertexArray(geometry["plane"]);

	// 1. Dibujar el plano relleno con color oscuro (azul oscuro)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform4f(uniforms["color"], 0.1f, 0.12f, 0.22f, 0.8f);
	glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());

	// 2. Dibujar la cuadrícula de alambre en color cian brillante con desplazamiento de polígono
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.5f);
	glUniform4f(uniforms["color"], 0.0f, 0.75f, 1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());
	glDisable(GL_POLYGON_OFFSET_LINE);
}

Application::~Application() 
{}