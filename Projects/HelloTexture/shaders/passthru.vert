#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

out vec2 texCoord;
uniform float time;
uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

void main ()
{  	
	gl_Position = projection * camera * modelTrans * vPosition;
}