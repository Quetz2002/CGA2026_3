#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
out vec3 Normal;

void main ()
{
	// Parámetros de la onda radial (gota de agua)
	float amp = 0.15;      // Amplitud de la onda
	float k = 15.0;        // Frecuencia espacial (número de anillos)
	float speed = 5.0;     // Velocidad de propagación de las ondas
	float alpha = 0.8;     // Coeficiente de atenuación/decaimiento hacia los bordes

	// Distancia desde el centro (0,0), evitando la división por cero
	float r = length(vPosition.xz);
	r = max(r, 0.0001);

	// Calcular posición y (gota cayendo en el centro con decaimiento exponencial)
	vec4 animatedPos = vPosition;
	float phase = k * r - speed * uTime;
	float decay = exp(-alpha * r);
	animatedPos.y = amp * decay * cos(phase);

	// Posición del fragmento en espacio del mundo
	FragPos = vec3(modelTrans * animatedPos);
	
	// Derivada de y respecto a la distancia radial r (dy/dr)
	float dy_dr = -amp * decay * (alpha * cos(phase) + k * sin(phase));
	
	// Derivadas parciales con respecto a X y Z usando la regla de la cadena
	float dy_dx = dy_dr * (vPosition.x / r);
	float dy_dz = dy_dr * (vPosition.z / r);
	vec3 localNormal = vec3(-dy_dx, 1.0, -dy_dz);
	
	mat3 normalMatrix = transpose(inverse(mat3(modelTrans)));
	Normal = normalMatrix * localNormal; // Normalización se hace por fragmento
	
	gl_Position = projection * camera * modelTrans * animatedPos;
}