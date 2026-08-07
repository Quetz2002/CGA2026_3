#version 460 core

layout (location = 0) in vec4 vPosition;

uniform float time;
uniform float amp;       // Nuevo uniform para amplitud
uniform float frq;       // Nuevo uniform para frecuencia
uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
}

struct light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
}

uniform Material uMaterial;

uniform light uLight;

float F(float x, float z, float amp, float fase, float frq)
{
    float distance = sqrt(pow(x, 2.0) + pow(z, 2.0));
    return sin(distance * frq - fase) * amp;
}

void main ()
{
    vec4 newPosition = vPosition;
    float fase = time * 5.0f;
    
    // Usamos los uniforms amp y frq aquí
    newPosition.y = F(newPosition.x, newPosition.z, amp, fase, frq);
    
    gl_Position = projection * camera * modelTrans * newPosition;
}
