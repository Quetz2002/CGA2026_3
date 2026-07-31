#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;

out vec4 color;
uniform float time;


void main ()
{  	
	color = vColor;
	vec4 newPosition = vPosition;

	newPosition.x *= cos(time * 3.141592f / 180.0);
	gl_Position = newPosition; //equivale a hacer return
}