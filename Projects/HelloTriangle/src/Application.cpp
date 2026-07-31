#include "Application.h"
#include <vector>
#include "ShaderFuncs.h"

void Application::setupTriangle()
{
	std::vector<GLfloat> triangle
	{
		-1.0f, 1.0f, -1.0f, 1.0f, // vertice 0
		1.0f, 0.0f, 0.0f, 1.0f,  //RED
		-1.0f, -1.0f, -1.0f, 1.0f,// vertice 1

		0.0f, 1.0f, 0.0f, 1.0f,	 //GREEN
		1.0f, -1.0f, -1.0f, 1.0f, // vertice 2
		0.0f, 0.0f, 1.0f, 1.0f	//BLUE
	};

	//Crear VAO
	GLuint VAO_id, VBO_id;
	glGenVertexArrays(1, &VAO_id);
	glBindVertexArray(VAO_id);

	geometry["triangulo"] = VAO_id;

	//crear VBO vertices
	glGenBuffers(1, &VBO_id);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_id);  //Ojo esto todavia no ha reservado memoria
	//Pasar arreglo de vertices
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * triangle.size(),
		&triangle[0],
		GL_STATIC_DRAW);
	
	
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); //geometria
	//glEnableVertexAttribArray(0);
	//unsigned int vertexSizenBytes = sizeof(GL_FLOAT) * 4 * 3; //4 flotantes por vertices y 3 vertices de geometria
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertexSizenBytes)); //Color
	
	GLsizei stride = sizeof(GLfloat) * 8;
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(GLfloat) * 4));
	glEnableVertexAttribArray(1);
}

void Application::setupShaders()
{
	std::string vertexShader = loadTextFile("shaders/passthru.vert");
	std::string fragmentShader = loadTextFile("shaders/passthru.frag");
	programs["passthru"] = InitializeProgram(vertexShader, fragmentShader);
}

void Application::setup()
{
	//Crear Geometria pasarla al GPU y borrar de la RAM
	setupTriangle();
	
	//Cargar shaders, compilarlos y ligarlos
	setupShaders();
}

void Application::update()
{}

void Application::draw() 
{
	//Seleccionar programa (shaders)
	glUseProgram(programs["passthru"]);

	//Seleccionar la geometria (el triangulo)
	glBindVertexArray(geometry["triangulo"]);

	//glDraw()
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

Application::~Application() 
{}