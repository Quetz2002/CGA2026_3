# README: Gouraud Shading en Proyecto ADS

Este archivo documenta la implementación del modelo de iluminación ADS (Ambient-Diffuse-Specular) usando **Shading de Gouraud** en el proyecto `ADS`. El proyecto está configurado bajo una lógica de archivos y carga invertidos en C++ (donde `.frag` actúa como Vertex Shader y `.vert` actúa como Fragment Shader), con una fuente de luz estática.

---

## 1. Explicación del Modelo de Iluminación de Gouraud (ADS por Vértice)

A diferencia del Shading de Phong, donde los cálculos de iluminación se realizan por cada píxel/fragmento en el Fragment Shader, en el **Shading de Gouraud** toda la iluminación ADS (Ambiental, Difusa y Especular) se calcula en el **Vertex Shader** para cada vértice de la geometría.

### Funcionamiento:
1. **Cálculo por Vértice**: El Vertex Shader calcula el color total para cada vértice sumando los componentes ambiental, difuso y especular:
   $$\text{Color del Vértice} = I_A + I_D + I_S$$
2. **Interpolación**: La GPU toma los colores calculados en cada vértice del triángulo y los interpola de manera lineal a través de toda la superficie del polígono.
3. **Fragment Shader Passthru**: El Fragment Shader simplemente recibe el color ya interpolado y lo dibuja directamente en la pantalla.

### Comparativa: Gouraud vs. Phong
- **Rendimiento**: Gouraud es computacionalmente más rápido y ligero porque los cálculos de iluminación ocurren una vez por vértice en lugar de una vez por fragmento (píxel). En mallas de baja resolución, Gouraud tiene menos costo que Phong.
- **Calidad Visual**: Debido a la interpolación lineal del color, los brillos especulares muy definidos pueden "perderse" o verse distorsionados (facetados) si no hay suficientes vértices en esa zona. El sombreado de Phong produce brillos especulares mucho más suaves y redondeados porque evalúa las normales por fragmento.

---

## 2. Lógica de Archivos y Carga Invertida

- **Estructura física**:
  - [Gouraud.frag](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/ADS/shaders/Gouraud.frag): Contiene el código del **Vertex Shader** de Gouraud (incluyendo la lógica matemática de ADS y la transformación de posiciones).
  - [Gouraud.vert](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/ADS/shaders/Gouraud.vert): Contiene el código del **Fragment Shader** de Gouraud (que solo pasa el color interpolado `GouraudColor` al búfer de salida).
- **Carga en C++**:
  - En [Application.cpp](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/ADS/src/Application.cpp), la variable `vertexShader` se carga desde `Gouraud.frag` y se compila como `GL_VERTEX_SHADER`.
  - La variable `fragmentShader` se carga desde `Gouraud.vert` y se compila como `GL_FRAGMENT_SHADER`.

---

## 3. Código Completo de los Shaders

### [Gouraud.frag](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/ADS/shaders/Gouraud.frag) (Vertex Shader de Gouraud)
```glsl
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

// Componente Ambiental
vec4 computeAmbient(Material mat, Light lit) {
    return mat.ambient * lit.ambient;
}

// Componente Difuso
vec4 computeDiffuse(Material mat, Light lit, vec3 normal, vec3 lightDir) {
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * mat.diffuse * lit.diffuse;
}

// Componente Especular
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
```

### [Gouraud.vert](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/ADS/shaders/Gouraud.vert) (Fragment Shader de Gouraud)
```glsl
#version 460 core

out vec4 fragColor;
in vec4 GouraudColor;

void main()
{
    fragColor = GouraudColor;
}
```

---

## 4. Modificaciones Realizadas en el Código C++ y Proyecto

### [Application.cpp](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/ADS/src/Application.cpp)

#### Modificaciones en `setupShaders()` (Carga de Gouraud con nombres y rutas actualizadas):
```cpp
void Application::setupShaders()
{
	// Carga invertida: el vertex shader está en .frag y el fragment shader en .vert
	std::string vertexShader = loadTextFile("shaders/Gouraud.frag");
	std::string fragmentShader = loadTextFile("shaders/Gouraud.vert");
	programs["Gouraud"] = InitializeProgram(vertexShader, fragmentShader);

	// Obtener localidades de variables uniform de matrices y color
	uniforms["camera"] = glGetUniformLocation(programs["Gouraud"], "camera");
	uniforms["modelTrans"] = glGetUniformLocation(programs["Gouraud"], "modelTrans");
	uniforms["projection"] = glGetUniformLocation(programs["Gouraud"], "projection");
	uniforms["color"] = glGetUniformLocation(programs["Gouraud"], "color");

	// Obtener localidades de variables uniform de la estructura Material
	uniforms["uMaterial.ambient"] = glGetUniformLocation(programs["Gouraud"], "uMaterial.ambient");
	uniforms["uMaterial.diffuse"] = glGetUniformLocation(programs["Gouraud"], "uMaterial.diffuse");
	uniforms["uMaterial.specular"] = glGetUniformLocation(programs["Gouraud"], "uMaterial.specular");
	uniforms["uMaterial.shininess"] = glGetUniformLocation(programs["Gouraud"], "uMaterial.shininess");

	// Obtener localidades de variables uniform de la estructura Light
	uniforms["uLight.ambient"] = glGetUniformLocation(programs["Gouraud"], "uLight.ambient");
	uniforms["uLight.diffuse"] = glGetUniformLocation(programs["Gouraud"], "uLight.diffuse");
	uniforms["uLight.specular"] = glGetUniformLocation(programs["Gouraud"], "uLight.specular");
	uniforms["uLight.position"] = glGetUniformLocation(programs["Gouraud"], "uLight.position");

	// Uniforms de cámara/iluminación auxiliar
	uniforms["uViewPos"] = glGetUniformLocation(programs["Gouraud"], "uViewPos");
	uniforms["uUseLighting"] = glGetUniformLocation(programs["Gouraud"], "uUseLighting");
}
```

#### Modificación en `update()` (Fijar la iluminación en `(0.0f, 2.5f, 0.0f)`):
```cpp
	// Luz fija sobre el plano para una iluminación constante
	light.position = glm::vec3(0.0f, 2.5f, 0.0f);
```

#### Modificación en `draw()`:
```cpp
void Application::draw() 
{
	// Seleccionar programa (shaders)
	glUseProgram(programs["Gouraud"]);
    // ...
```

---

## 5. Configuración del Proyecto en Visual Studio

El archivo de configuración [Plane.vcxproj](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/ADS/Plane.vcxproj) y los filtros [Plane.vcxproj.filters](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/ADS/Plane.vcxproj.filters) se han actualizado para incluir correctamente las referencias a los nuevos shaders:
- `shaders/Gouraud.vert`
- `shaders/Gouraud.frag`
- `shaders/PhongShadding.vert`
- `shaders/PhongShadding.frag`
