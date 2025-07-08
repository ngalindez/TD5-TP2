---

# Solucionador de CVRP

Esta herramienta de línea de comandos permite resolver el Problema de Ruteo de Vehículos con Capacidad (CVRP) a partir de instancias en formato VRPLIB, combinando varias heurísticas y metaheurísticas y opciones de búsqueda local.

---

## 📋 Contenido

- [Características](#-características)
- [Requisitos](#-requisitos)
- [Construcción](#-construcción)
- [Uso](#-uso)
- [Ejemplo](#-ejemplo)
- [Notas](#-notas)
- [Estructura del proyecto](#-estructura-del-proyecto)

---

## 🚀 Características

### Heurísticas

- Clarke & Wright
- Inserción Más Cercana

### Metaheurística

- GRASP (con base en Inserción Más Cercana)

### Búsqueda Local

- Operador Swap
- Operador Relocate
- Ambos combinados

### Formato de entrada

- Instancias estándar VRPLIB

### Salida

- Costo total de la solución
- Número de rutas
- (Opcional) Listado completo de rutas
- Tiempos de cálculo

---

## 🛠 Requisitos

- Compilador C++17 (g++, clang++)
- CMake (versión 3.10 o superior)
- Make

---

## 📦 Construcción

```bash
mkdir build
cd build
cmake ..
make
```
El ejecutable principal (`cvrp_solver`) quedará en el directorio `build/`.

---

## 💻 Uso

### 1. Interactivo

```bash
./build/cvrp_solver
```
Introduce la ruta al archivo `.vrp`.

Navega por el menú en pantalla para elegir heurística, metaheurística y/o operadores de búsqueda local.

Opcionalmente, tras cada paso puedes elegir mostrar o no el detalle completo de rutas.

### 2. Por script (`run.sh`)

```bash
./run.sh <ruta_instancia.vrp>
```
Compila (si no está compilado) y ejecuta el solver sobre la instancia indicada.

Ideal para ejecuciones por lotes o experimentos automatizados.

La salida incluye líneas con `COST:` y `TIME:`, fáciles de parsear.

---

## 📝 Ejemplo

```bash
$ ./run.sh instancias/2l-cvrp-0/E200-17C.vrp
```

```
Ingrese ruta al archivo VRP: instancias/2l-cvrp-0/E200-17C.vrp

=== Menú Principal ===
1. Clarke & Wright
2. Inserción Más Cercana
3. GRASP
4. Salir
Seleccione: 2

[Inserción Cercana] Costo=2199.02  Rutas=14  Tiempo=0.12s
¿Mostrar rutas completas? (s/n): n

=== Búsqueda Local para Inserción Cercana ===
1. Operador Swap
2. Operador Relocate
3. Ambos operadores
4. Volver al menú principal
Seleccione: 4

```

---

## ⚠️ Notas

- **Errores de compilación:** Verifica que dispones de un compilador C++17 y CMake instalado.
- **Fallos por segmentación:** Comprueba que tu archivo de instancia respeta el formato VRPLIB y que los IDs de nodo no desbordan las dimensiones de la matriz de distancias.
- Para extender o modificar heurísticas y operadores, explora el código en `src/`.

---

## 📁 Estructura del proyecto

```
.
├── CMakeLists.txt
├── run.sh              # Script de ayuda para ejecución por lotes
├── instancias/         
├── src/                # Código fuente
│   ├── Cliente.cpp/.h
│   ├── Ruta.cpp/.h
│   ├── Solucion.cpp/.h
│   ├── VRPLIBReader.cpp/.h
│   ├── HeuristicaClarkeWright.cpp/.h
│   ├── HeuristicaInsercionCercana.cpp/.h
│   ├── GRASP.cpp/.h
│   ├── OperadorSwap.cpp/.h
│   ├── OperadorRelocate.cpp/.h
│   └── CWwLocalSearch.cpp/.h
└── build/              # Archivos generados tras cmake & make