# README: Gouraud & Phong Shading en Proyecto GouraudPhongShading

Este proyecto implementa el modelo de iluminación **ADS** (Ambient-Diffuse-Specular) en un plano 3D dinámico que simula ondas radiales de agua (ondas concéntricas generadas por una gota). El sombreado se realiza a través de dos técnicas: **Gouraud Shading** (iluminación por vértice) y **Phong Shading** (iluminación por fragmento), permitiendo alternar entre ambas en tiempo de ejecución.

---

## 1. Simulación Física: Sábana Dinámica (Gota de Agua)

Para lograr un comportamiento de ondas radiales concéntricas propagándose desde el centro hacia los extremos y atenuándose progresivamente (como una gota real), se implementó la siguiente ecuación:

$$y = A \cdot e^{-\alpha r} \cdot \cos(k \cdot r - \omega t)$$

### Parámetros de la Onda:
- **Amplitud ($A = 0.15$)**: Define la altura máxima de la ola en el centro.
- **Frecuencia espacial ($k = 15.0$)**: Define el número de crestas y valles (anillos concéntricos).
- **Velocidad ($\omega = 5.0$)**: Define la velocidad de propagación radial hacia los bordes.
- **Atenuación ($\alpha = 0.8$)**: Coeficiente de decaimiento exponencial que disipa las ondas conforme se alejan del origen.
- **Distancia radial ($r = \sqrt{x^2 + z^2}$)**: Distancia Euclidiana desde el vértice hasta el centro $(0,0)$.

### Cálculo Analítico de la Normal:
Dado que la sábana está animada dinámicamente en la GPU, las normales precalculadas del plano plano $(0, 1, 0)$ no reflejan la deformación. Para obtener la iluminación ADS correcta, derivamos analíticamente la ecuación de la onda usando la regla de la cadena:

$$\frac{\partial y}{\partial x} = \frac{\partial y}{\partial r} \frac{x}{r}, \quad \frac{\partial y}{\partial z} = \frac{\partial y}{\partial r} \frac{z}{r}$$

Donde la derivada radial es:

$$\frac{\partial y}{\partial r} = -A \cdot e^{-\alpha r} \left[ \alpha \cos(k \cdot r - \omega t) + k \sin(k \cdot r - \omega t) \right]$$

El vector normal en el espacio del objeto resulta en:

$$\mathbf{N}_{\text{local}} = \left( -\frac{\partial y}{\partial x}, 1.0, -\frac{\partial y}{\partial z} \right)$$

Este vector se multiplica por la matriz normal (`normalMatrix`) en el Vertex Shader para adaptarlo al espacio del mundo.

---

## 2. Lógica de Carga y Nombres Invertidos de Shaders

Debido a la estructura del cargador en C++, este proyecto implementa una lógica de carga y archivos cruzados/invertidos:
- Los archivos `.frag` actúan físicamente como **Vertex Shaders**.
- Los archivos `.vert` actúan físicamente como **Fragment Shaders**.

Además, se conservaron los nombres con errores de tipeo originales para compatibilidad del cargador:

1. **Gouraud Shading (Iluminación por Vértice)**:
   - **Vertex Shader**: [shaders/PhongShadding.frag](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/GouraudPhongShading/shaders/PhongShadding.frag) (Calcula la ecuación ADS y el color resultante en el vértice; exporta `GouraudColor`).
   - **Fragment Shader**: [shaders/PhongShadding.vert](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/GouraudPhongShading/shaders/PhongShadding.vert) (Recibe el color interpolado y lo asigna a `fragColor`).

2. **Phong Shading (Iluminación por Fragmento)**:
   - **Vertex Shader**: [shaders/Gourud.frag](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/GouraudPhongShading/shaders/Gourud.frag) (Calcula la onda radial y pasa la normal del vértice `Normal` y la posición `FragPos` al fragmento).
   - **Fragment Shader**: [shaders/Gouraud.vert](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/GouraudPhongShading/shaders/Gouraud.vert) (Interpola las normales y calcula la iluminación ADS por cada píxel para brillos suaves).

---

## 3. Integración en C++ y Teclado

### Alternancia de Shaders (Tecla "S"):
- En [HelloGouraudPhongShading.cpp](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/GouraudPhongShading/src/HelloGouraudPhongShading.cpp), registramos una condición en el callback de GLFW para alternar la variable bandera `app.usePhong` al presionar la tecla **"S"**:
  ```cpp
  if (key == GLFW_KEY_S && action == GLFW_PRESS)
  {
      app.usePhong = !app.usePhong;
      std::cout << "Shader cambiado a: " << (app.usePhong ? "Phong Shading" : "Gouraud Shading") << std::endl;
  }
  ```

### Estructura de Uniforms Dinámicos:
- En [Application.cpp](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/GouraudPhongShading/src/Application.cpp), el mapeo de uniformes se realiza prefijando las variables (`Gouraud_` y `Phong_`). 
- Durante el método `draw()`, la aplicación activa el shader seleccionado, pasa la variable `uTime` (tiempo transcurrido en segundos) y los parámetros del material/luz correspondientes.

---

## 4. Ajustar la Resolución del Plano (Subdivisiones)

Por defecto, el plano se crea con un valor bajo de subdivisiones. Para aumentar la densidad de triángulos y ver las ondas con máxima fluidez y suavidad, debes dirigirte a [Application.cpp](file:///c:/Users/Gio/Documents/GitHub/Original/CGA2026_3/Projects/GouraudPhongShading/src/Application.cpp) y buscar la línea en `Application::setup()`:

```cpp
void Application::setup()
{
	// Crear Plano
	plane.createPlane(10); // <-- Modifica este número
```

### Guía de Valores Recomendados:
- **`10`** (Predeterminado): $10 \times 10$ subdivisiones (200 triángulos). Ligero pero geométricamente brusco.
- **`100`**: $100 \times 100$ subdivisiones ($20,000$ triángulos). Buena fluidez de oleaje.
- **`150` a `200`** (Recomendado): $150 \times 150$ subdivisiones ($45,000$ triángulos). **Visualmente espectacular**. Las ondas concéntricas se ven completamente redondeadas y el brillo especular de Phong se desplaza suavemente de píxel a píxel.
