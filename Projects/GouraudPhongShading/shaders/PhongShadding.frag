#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 modelTrans;
uniform mat4 camera;
uniform mat4 projection;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct Light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};

uniform Material uMaterial;
uniform Light uLight;
uniform vec3 uViewPos;
uniform int uUseLighting; // 1 para usar iluminación ADS, 0 para color plano (wireframe)
uniform vec4 color;       // Color plano de fallback para wireframe
uniform float uTime;      // Tiempo para animar la sábana

out vec4 GouraudColor;

// Función independiente para el componente Ambiental
vec4 computeAmbient(Material mat, Light lit) {
    return mat.ambient * lit.ambient;
}

// Función independiente para el componente Difuso
vec4 computeDiffuse(Material mat, Light lit, vec3 normal, vec3 lightDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * mat.diffuse * lit.diffuse;
}

// Función independiente para el componente Especular
vec4 computeSpecular(Material mat, Light lit, vec3 normal, vec3 lightDir, vec3 viewDir) {
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    return spec * mat.specular * lit.specular;
}

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

    gl_Position = projection * camera * modelTrans * animatedPos;

    if (uUseLighting != 0) {
        // Posición en espacio del mundo
        vec3 FragPos = vec3(modelTrans * animatedPos);
        
        // Derivada de y respecto a la distancia radial r (dy/dr)
        float dy_dr = -amp * decay * (alpha * cos(phase) + k * sin(phase));
        
        // Derivadas parciales con respecto a X y Z usando la regla de la cadena
        float dy_dx = dy_dr * (vPosition.x / r);
        float dy_dz = dy_dr * (vPosition.z / r);
        vec3 localNormal = vec3(-dy_dx, 1.0, -dy_dz);
        
        mat3 normalMatrix = transpose(inverse(mat3(modelTrans)));
        vec3 norm = normalize(normalMatrix * localNormal);
        
        vec3 lightDir = normalize(uLight.position - FragPos);
        vec3 viewDir = normalize(uViewPos - FragPos);

        vec4 ambient = computeAmbient(uMaterial, uLight);
        vec4 diffuse = computeDiffuse(uMaterial, uLight, norm, lightDir);
        vec4 specular = computeSpecular(uMaterial, uLight, norm, lightDir, viewDir);

        vec4 result = ambient + diffuse + specular;
        result.a = uMaterial.diffuse.a; // Mantener la opacidad del material
        GouraudColor = result;
    } else {
        GouraudColor = color;
    }
}
