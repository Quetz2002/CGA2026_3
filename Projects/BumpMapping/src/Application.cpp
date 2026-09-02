#define STB_IMAGE_IMPLEMENTATION

#include "Application.h"
#include <GLFW/glfw3.h>
#include <vector>
#include "ShaderFuncs.h"
#include <chrono>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

GLuint Application::setupTexture(const std::string& filename)
{
	int width, height, channels;
	unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 4);
	if (img == nullptr)
		return -1;
	GLuint texID = -1;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

	stbi_image_free(img);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}

void Application::setupShaders()
{
	// 1. COMPILAR PROGRAMA GOURAUD
	// Carga invertida: el vertex shader está en .frag y el fragment shader en .vert
	std::string vGouraud = loadTextFile("shaders/PhongShadding.frag");
	std::string fGouraud = loadTextFile("shaders/PhongShadding.vert");
	programs["Gouraud"] = InitializeProgram(vGouraud, fGouraud);

	// 2. COMPILAR PROGRAMA PHONG
	std::string vPhong = loadTextFile("shaders/Gourud.frag");
	std::string fPhong = loadTextFile("shaders/Gouraud.vert");
	programs["Phong"] = InitializeProgram(vPhong, fPhong);

	// 3. OBTENER UNIFORMS PARA GOURAUD
	uniforms["Gouraud_camera"] = glGetUniformLocation(programs["Gouraud"], "camera");
	uniforms["Gouraud_modelTrans"] = glGetUniformLocation(programs["Gouraud"], "modelTrans");
	uniforms["Gouraud_projection"] = glGetUniformLocation(programs["Gouraud"], "projection");
	uniforms["Gouraud_color"] = glGetUniformLocation(programs["Gouraud"], "color");
	uniforms["Gouraud_uMaterial.ambient"] = glGetUniformLocation(programs["Gouraud"], "uMaterial.ambient");
	uniforms["Gouraud_uMaterial.diffuse"] = glGetUniformLocation(programs["Gouraud"], "uMaterial.diffuse");
	uniforms["Gouraud_uMaterial.specular"] = glGetUniformLocation(programs["Gouraud"], "uMaterial.specular");
	uniforms["Gouraud_uMaterial.shininess"] = glGetUniformLocation(programs["Gouraud"], "uMaterial.shininess");
	uniforms["Gouraud_uLight.ambient"] = glGetUniformLocation(programs["Gouraud"], "uLight.ambient");
	uniforms["Gouraud_uLight.diffuse"] = glGetUniformLocation(programs["Gouraud"], "uLight.diffuse");
	uniforms["Gouraud_uLight.specular"] = glGetUniformLocation(programs["Gouraud"], "uLight.specular");
	uniforms["Gouraud_uLight.position"] = glGetUniformLocation(programs["Gouraud"], "uLight.position");
	uniforms["Gouraud_uViewPos"] = glGetUniformLocation(programs["Gouraud"], "uViewPos");
	uniforms["Gouraud_uUseLighting"] = glGetUniformLocation(programs["Gouraud"], "uUseLighting");
	uniforms["Gouraud_uTime"] = glGetUniformLocation(programs["Gouraud"], "uTime");

	// 4. OBTENER UNIFORMS PARA PHONG
	uniforms["Phong_camera"] = glGetUniformLocation(programs["Phong"], "camera");
	uniforms["Phong_modelTrans"] = glGetUniformLocation(programs["Phong"], "modelTrans");
	uniforms["Phong_projection"] = glGetUniformLocation(programs["Phong"], "projection");
	uniforms["Phong_color"] = glGetUniformLocation(programs["Phong"], "color");
	uniforms["Phong_uMaterial.ambient"] = glGetUniformLocation(programs["Phong"], "uMaterial.ambient");
	uniforms["Phong_uMaterial.diffuse"] = glGetUniformLocation(programs["Phong"], "uMaterial.diffuse");
	uniforms["Phong_uMaterial.specular"] = glGetUniformLocation(programs["Phong"], "uMaterial.specular");
	uniforms["Phong_uMaterial.shininess"] = glGetUniformLocation(programs["Phong"], "uMaterial.shininess");
	uniforms["Phong_uLight.ambient"] = glGetUniformLocation(programs["Phong"], "uLight.ambient");
	uniforms["Phong_uLight.diffuse"] = glGetUniformLocation(programs["Phong"], "uLight.diffuse");
	uniforms["Phong_uLight.specular"] = glGetUniformLocation(programs["Phong"], "uLight.specular");
	uniforms["Phong_uLight.position"] = glGetUniformLocation(programs["Phong"], "uLight.position");
	uniforms["Phong_uViewPos"] = glGetUniformLocation(programs["Phong"], "uViewPos");
	uniforms["Phong_uUseLighting"] = glGetUniformLocation(programs["Phong"], "uUseLighting");
	uniforms["Phong_uTime"] = glGetUniformLocation(programs["Phong"], "uTime");
}

void Application::setup()
{
	// Crear Plano
	setupShaders();
	plane.createPlane(10);
	plane.cleanMemory();
	geometry["plane"] = plane.vao;	// Cargar shaders, compilarlos y ligarlos

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

	// Luz fija sobre el plano para una iluminación constante
	light.position = glm::vec3(0.0f, 2.5f, 0.0f);

	float aspect = height > 0 ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
	projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void Application::draw() 
{
	std::string prefix = usePhong ? "Phong_" : "Gouraud_";
	GLuint program = usePhong ? programs["Phong"] : programs["Gouraud"];

	// Seleccionar programa (shaders)
	glUseProgram(program);

	// Matrices
	glUniformMatrix4fv(uniforms[prefix + "camera"], 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(uniforms[prefix + "modelTrans"], 1, GL_FALSE, glm::value_ptr(modelTrans));
	glUniformMatrix4fv(uniforms[prefix + "projection"], 1, GL_FALSE, glm::value_ptr(projection));

	// Pasar valores de Material
	glUniform4fv(uniforms[prefix + "uMaterial.ambient"], 1, glm::value_ptr(material.ambient));
	glUniform4fv(uniforms[prefix + "uMaterial.diffuse"], 1, glm::value_ptr(material.diffuse));
	glUniform4fv(uniforms[prefix + "uMaterial.specular"], 1, glm::value_ptr(material.specular));
	glUniform1f(uniforms[prefix + "uMaterial.shininess"], material.shininess);

	// Pasar valores de Luz
	glUniform4fv(uniforms[prefix + "uLight.ambient"], 1, glm::value_ptr(light.ambient));
	glUniform4fv(uniforms[prefix + "uLight.diffuse"], 1, glm::value_ptr(light.diffuse));
	glUniform4fv(uniforms[prefix + "uLight.specular"], 1, glm::value_ptr(light.specular));
	glUniform3fv(uniforms[prefix + "uLight.position"], 1, glm::value_ptr(light.position));

	// Pasar posición de cámara
	glUniform3fv(uniforms[prefix + "uViewPos"], 1, glm::value_ptr(viewPos));

	// Pasar tiempo para la sábana animada (en segundos)
	glUniform1f(uniforms[prefix + "uTime"], static_cast<float>(time * 0.001));

	// Seleccionar la geometria
	glBindVertexArray(geometry["plane"]);

	// 1. Dibujar el plano relleno con el modelo ADS activado
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform1i(uniforms[prefix + "uUseLighting"], 1);
	glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());

	// 2. Dibujar la cuadrícula de alambre en color cian brillante (sin modelo de iluminación ADS)
	glLineWidth(1.5f);
	glUniform1i(uniforms[prefix + "uUseLighting"], 0);
	glUniform4f(uniforms[prefix + "color"], 0.0f, 0.75f, 1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, plane.getNumVertex());
}

Application::~Application() 
{}