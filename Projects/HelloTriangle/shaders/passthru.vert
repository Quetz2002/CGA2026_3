#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;

out vec4 color;


void main ()
{  	
	//color = vec4(0.0, 0.5, 0.8, 0.0);
	color = vColor;
	gl_Position = vPosition; //equivale a hacer return
}