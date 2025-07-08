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

### Por script 'run.sh'

```bash
./run.sh 
```
Dara la lista de comandos disponibles para usar el codigo.

Navega por el menú en pantalla para elegir heurística, metaheurística y/o operadores de búsqueda local.

Opcionalmente, tras cada paso puedes elegir mostrar o no el detalle completo de rutas.

---

## 📝 Ejemplo

```bash
$ ./run.sh run
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
