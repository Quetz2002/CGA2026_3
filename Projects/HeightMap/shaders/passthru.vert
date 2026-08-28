#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoords;

uniform float time;
uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

out vec2 texCoords;

void main ()
{
	texCoords = vTexCoords;
vec4 texel0 = texture(tex0, texCoords);
	gl_Position = projection * camera * modelTrans * vPosition;
}