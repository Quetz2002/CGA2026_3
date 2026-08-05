#version 460 core

layout (location = 0) in vec4 vPosition;

uniform float time;



void main ()
{  	
	vec4 newPosition;
	newPosition.x = vPosition.x;
	newPosition.y = vPosition.z;
	newPosition.z = vPosition.y;
	gl_Position = newPosition;
}