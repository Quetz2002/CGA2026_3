#pragma once
#include <map>
#include <string>
#include "glad/glad.h"
#include "Plane.h"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"

struct Material {
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;
};

struct Light {
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec3 position;
};

class Application
{
public:
	void setup();
	void update(struct GLFWwindow* window);
	void draw();
	~Application();

	std::map<std::string, GLuint> geometry;
	std::map<std::string, GLuint> programs;
	std::map<std::string, GLuint> uniforms;
	std::map<std::string, GLuint> textures;

	glm::mat4 camera;
	glm::mat4 modelTrans;
	glm::mat4 projection;

	double time{ 0.0f };

	Plane plane;

	Material material;
	Light light;
	glm::vec3 viewPos;

	bool usePhong{ false };

private:
	void setupShaders();
	GLuint setupTexture(const std::string& filename);

	double lastTime{ 0.0 };
	float currentRoll{ 0.0f };
	float currentPitch{ 0.0f };
	float currentYaw{ 0.0f };
};