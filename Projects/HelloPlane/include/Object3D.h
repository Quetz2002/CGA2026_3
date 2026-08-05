#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

class Object3D
{
public:
	GLuint	vao, //Vertex Array Object
			vbo; //vertex buffer object

	int numVertex; //numero de vertices para este objeto
	glm::vec3 angles;

	GLuint shader[2]; //referencia a los shaders
	GLuint transforms,
			wireframe;

	//Object3D();

};
