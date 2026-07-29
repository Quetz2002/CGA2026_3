#pragma once
#include "glad/glad.h"
#include <vector>
#include <string>


GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile);
GLuint CreateProgram(const std::vector<GLuint> &shaderList);
GLuint InitializeProgram(std::string strVertexShader, std::string strFragmentShader);
std::string loadTextFile(const std::string& filename);

