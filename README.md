# 🚚 Solucionador de CVRP

Herramienta de línea de comandos para resolver el **Problema de Ruteo de Vehículos con Capacidad (CVRP)** a partir de instancias en formato VRPLIB. Combina heurísticas, metaheurísticas y operadores de búsqueda local para explorar soluciones de calidad.

---

## 📑 Tabla de Contenidos

- [Características](#características)
- [Requisitos](#requisitos)
- [Construcción](#construcción)
- [Uso](#uso)
- [Ejemplo de ejecución](#ejemplo-de-ejecución)
- [Estructura del proyecto](#estructura-del-proyecto)

---

## ✨ Características

- **Heurísticas:**
  - Clarke & Wright
  - Inserción Más Cercana
- **Metaheurística:**
  - GRASP (basada en Inserción Más Cercana)
- **Búsqueda Local:**
  - Operador Swap
  - Operador Relocate
  - Combinación de ambos operadores
- **Entrada:**  
  - Instancias estándar VRPLIB
- **Salida:**
  - Costo total de la solución
  - Número de rutas generadas
  - (Opcional) Listado completo de rutas
  - Tiempos de cálculo

---

## ⚙️ Requisitos

- Compilador C++17 (`g++`, `clang++`)
- [CMake](https://cmake.org/) (≥ 3.10)
- Make

---

## 🛠 Construcción

Compila todo el proyecto con:

```bash
./run.sh build
```

Limpia el directorio de build si necesitás empezar de cero:

```bash
./run.sh clean
```

---

## 🖥️ Uso

Mostrá el menú de comandos disponibles con:

```bash
./run.sh
```

### Comandos principales

- `build` — Compila el proyecto
- `clean` — Limpia el build
- `run` — Ejecuta la app principal y pide el nombre de la instancia
- `instance <nombre>` — Corre el solver directamente en una instancia específica
- `test` — Ejecuta todos los tests
- `test-heuristics` — Solo tests de heurísticas
- `test-local` — Solo tests de búsqueda local
- `test-insertion` — Solo tests de inserción
- `test-operators` — Solo tests de operadores
- `help` — Muestra ayuda

### Ejemplo de uso rápido

1. **Compilá el proyecto**  
   ```bash
   ./run.sh build
   ```

2. **Ejecutá el menú interactivo**  
   ```bash
   ./run.sh run
   ```

   O directamente en una instancia:
   ```bash
   ./run.sh instance <nombre_instancia>
   ```

3. **Correr todos los tests**  
   ```bash
   ./run.sh test
   ```

   O tests específicos:
   ```bash
   ./run.sh test-heuristics
   ./run.sh test-local
   ./run.sh test-insertion
   ./run.sh test-operators
   ```

---

## 🧑‍💻 Ejemplo de ejecución

Menú principal:

```
=== Menú Principal ===
1. Clarke & Wright
2. Inserción Más Cercana
3. GRASP
4. Salir
Seleccione:
```

Resumen tras ejecutar una heurística:

```
[Inserción Cercana] Costo=1454.11  Rutas=16  Tiempo=0.0123693s
¿Mostrar rutas completas? (s/n):
```

Ejemplo de salida de rutas:

```
Ruta 1: 1 7 62 34 106 97 189 17 118 4 56 45 107 1 (Dem: 197, Costo: 58.24)
Ruta 2: 1 153 55 31 140 3 158 87 113 67 195 197 2 192 1 (Dem: 197, Costo: 59.57)
...
Ruta 16: 1 6 104 84 14 124 129 71 85 135 86 171 12 1 (Dem: 181, Costo: 126.60)
```

Selección de búsqueda local:

```
=== Búsqueda Local para Inserción Cercana ===
1. Operador Swap
2. Operador Relocate
3. Ambos operadores
4. Volver al menú principal
Seleccione:
```

Resultado tras aplicar búsqueda local:

```
[Swap+Reloc] Costo inicial=1454.11  Costo final=1437.37  Δ=16.73  Tiempo=1.58s
```

---

## 📂 Estructura del Proyecto

```
.
├── CMakeLists.txt
├── run.sh              # Script para construir/ejecutar/tests
├── instancias/         # Instancias VRPLIB de ejemplo
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
└── build/              # Archivos generados por cmake & make
```
