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
    gl_Position = projection * camera * modelTrans * vPosition;

    if (uUseLighting != 0) {
        // Posición del fragmento en espacio del mundo
        vec3 FragPos = vec3(modelTrans * vPosition);
        
        // Normal predefinida (0.0, 1.0, 0.0) orientada hacia arriba, transformada a espacio del mundo
        vec3 Normal = normalize(mat3(modelTrans) * vec3(0.0, 1.0, 0.0));
        
        vec3 norm = normalize(Normal);
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
