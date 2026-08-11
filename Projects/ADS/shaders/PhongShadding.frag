#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main ()
{
	// Posición del fragmento en espacio del mundo
	FragPos = vec3(modelTrans * vPosition);
	
	// Normal predefinida (0.0, 1.0, 0.0) orientada hacia arriba, transformada a espacio del mundo.
	// Dado que modelTrans solo realiza rotaciones, podemos usar directamente mat3(modelTrans).
	Normal = normalize(mat3(modelTrans) * vec3(0.0, 1.0, 0.0));
	
	gl_Position = projection * camera * modelTrans * vPosition;
}