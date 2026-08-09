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

	// Obtener localidades de variables uniform de matrices y color
	uniforms["camera"] = glGetUniformLocation(programs["passthru"], "camera");
	uniforms["modelTrans"] = glGetUniformLocation(programs["passthru"], "modelTrans");
	uniforms["projection"] = glGetUniformLocation(programs["passthru"], "projection");
	uniforms["color"] = glGetUniformLocation(programs["passthru"], "color");

	// Obtener localidades de variables uniform de la estructura Material
	uniforms["uMaterial.ambient"] = glGetUniformLocation(programs["passthru"], "uMaterial.ambient");
	uniforms["uMaterial.diffuse"] = glGetUniformLocation(programs["passthru"], "uMaterial.diffuse");
	uniforms["uMaterial.specular"] = glGetUniformLocation(programs["passthru"], "uMaterial.specular");
	uniforms["uMaterial.shininess"] = glGetUniformLocation(programs["passthru"], "uMaterial.shininess");

	// Obtener localidades de variables uniform de la estructura Light
	uniforms["uLight.ambient"] = glGetUniformLocation(programs["passthru"], "uLight.ambient");
	uniforms["uLight.diffuse"] = glGetUniformLocation(programs["passthru"], "uLight.diffuse");
	uniforms["uLight.specular"] = glGetUniformLocation(programs["passthru"], "uLight.specular");
	uniforms["uLight.position"] = glGetUniformLocation(programs["passthru"], "uLight.position");

	// Uniforms de cámara/iluminación auxiliar
	uniforms["uViewPos"] = glGetUniformLocation(programs["passthru"], "uViewPos");
	uniforms["uUseLighting"] = glGetUniformLocation(programs["passthru"], "uUseLighting");
}

void Application::setup()
{
	// Crear Plano
	plane.createPlane(100);
	plane.cleanMemory();
	geometry["plane"] = plane.vao;	// Cargar shaders, compilarlos y ligarlos
	setupShaders();

	// Inicializar material (color azul brillante metálico con alto brillo)
	material.ambient = glm::vec4(0.1f, 0.12f, 0.22f, 1.0f);
	material.diffuse = glm::vec4(0.15f, 0.45f, 0.9f, 1.0f);
	material.specular = glm::vec4(0.9f, 0.9f, 1.0f, 1.0f);
	material.shininess = 64.0f;

	// Inicializar luz (luz blanca estándar)
	light.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light.position = glm::vec3(0.0f, 2.5f, 0.0f); // Se actualizará en el update

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
	time = currentTime * 1000.0; // time in ms

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
	viewPos = eye;

	// Luz orbitando lentamente sobre el plano para apreciar los brillos especulares
	float angle = static_cast<float>(currentTime) * 0.7f;
	light.position = glm::vec3(3.0f * cos(angle), 2.5f, 3.0f * sin(angle));

	float aspect = height > 0 ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
	projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void Application::draw() 
{
	// Seleccionar programa (shaders)
	glUseProgram(programs["passthru"]);

	// Matrices
	glUniformMatrix4fv(uniforms["camera"], 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(uniforms["modelTrans"], 1, GL_FALSE, glm::value_ptr(modelTrans));
	glUniformMatrix4fv(uniforms["projection"], 1, GL_FALSE, glm::value_ptr(projection));

	// Pasar valores de Material
	glUniform4fv(uniforms["uMaterial.ambient"], 1, glm::value_ptr(material.ambient));
	glUniform4fv(uniforms["uMaterial.diffuse"], 1, glm::value_ptr(material.diffuse));
	glUniform4fv(uniforms["uMaterial.specular"], 1, glm::value_ptr(material.specular));
	glUniform1f(uniforms["uMaterial.shininess"], material.shininess);

	// Pasar valores de Luz
	glUniform4fv(uniforms["uLight.ambient"], 1, glm::value_ptr(light.ambient));
	glUniform4fv(uniforms["uLight.diffuse"], 1, glm::value_ptr(light.diffuse));
	glUniform4fv(uniforms["uLight.specular"], 1, glm::value_ptr(light.specular));
	glUniform3fv(uniforms["uLight.position"], 1, glm::value_ptr(light.position));

	// Pasar posición de cámara
	glUniform3fv(uniforms["uViewPos"], 1, glm::value_ptr(viewPos));

	// Seleccionar la geometria
	glBindVertexArray(geometry["plane"]);

	// 1. Dibujar el plano relleno con el modelo ADS activado
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform1i(uniforms["uUseLighting"], 1);
	glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());

	// 2. Dibujar la cuadrícula de alambre en color cian brillante (sin modelo de iluminación ADS)
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.5f);
	glUniform1i(uniforms["uUseLighting"], 0);
	glUniform4f(uniforms["color"], 0.0f, 0.75f, 1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());
	glDisable(GL_POLYGON_OFFSET_LINE);
}

Application::~Application() 
{}