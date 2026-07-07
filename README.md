# Comparación de Algoritmos SSSP: Dijkstra vs. BMSSP

**Universidad Nacional de San Antonio Abad del Cusco (UNSAAC)**
Escuela Profesional de Ingeniería Informática y de Sistemas
Curso: Algoritmos Avanzados — Grupo 2

Artículo científico (formato IEEE): [`articulo_IEEE.pdf`](./articulo_IEEE.pdf)

## Integrantes

| Integrante | Código |
|---|---|
| Luz Diana Ancasi Aymachoque | 215714 |
| Yhasmin Graciela Ccansaya Corrales | 230249 |
| Bryan Sebastian Chávez Luna | 215817 |
| Nohemi Sharit Cuba Castillo | 230250 |

## Descripción general

Este repositorio documenta la comparación experimental entre el algoritmo clásico de **Dijkstra** y el algoritmo determinista **BMSSP** (*Bounded Multi-Source Shortest Path*, Duan et al., STOC 2025), que rompe la barrera de ordenamiento clásica de Dijkstra en grafos dirigidos dispersos. Se replican y extienden los benchmarks de tres trabajos recientes (Castro et al. 2025, Valko et al. 2025, Lewis 2023), y se propone una optimización original sobre BMSSP: **BMSSP-DedupPivots+EarlyStop**.

Cada carpeta de este repositorio corresponde a una implementación o experimento distinto, con su propio README detallado. Este documento es el punto de entrada general.

## Estructura del repositorio

| Carpeta | Contenido | ¿Qué es? |
|---|---|---|
| **`aporte-modificacion-codigo-bmssp/`** | C++20, header-only, basado en la implementación de Castro et al. | **Aporte original del grupo**: optimización BMSSP-DedupPivots+EarlyStop sobre el código de Castro et al. Ver `README_IMPLEMENTACION.md` dentro de esta carpeta para el detalle de la modificación. |
| **`implementacin-4-Breaking the sorting barrier/BMSSP`** | C++ (Visual Studio 2022, `.sln`) | Traducción propia e independiente del algoritmo BMSSP (a partir de la referencia en Go de Duan et al.), usada para verificar correctitud sobre grafos pequeños (Sección III del artículo). |
| **`implementacion-1-lewis-2023-dijkstra-heaps/`** | C++17 | Réplica del experimento de Lewis (2023): Dijkstra con binary heap, Fibonacci heap y árbol autobalanceado (BST). |
| **`implementacion-2-rap2363-ssps/`** | Rust | Repositorio base (de terceros) con Dijkstra y BMSSP en Rust, usado para correr pruebas sobre grafos reales (OSM), incluyendo el grafo de Washington D.C. citado en el artículo. |
| **`implementacion-2-ln-bmssp-evaluation/`** | Python / Jupyter | Notebook de análisis experimental asociado al paper de BMSSP aplicado a la Lightning Network (Valko et al., 2025). |
| **`implementacion-3-castro-2025-bmssp-original/`** | C++20, header-only | Implementación original (sin modificar) de Castro et al. (2025), usada como código base para el benchmark extendido de la Tabla 2 del artículo. |

> **Nota:** las carpetas `implementacion-3-castro-2025-bmssp-original/` y `aporte-modificacion-codigo-bmssp/` parten del mismo código de Castro et al.; la diferencia es que la segunda incluye la modificación propia del grupo (DedupPivots+EarlyStop). No confundir una con otra al revisar el aporte original.

## Requisitos e instalación por módulo

| Módulo | Requisitos |
|---|---|
| `aporte-modificacion-codigo-bmssp/`, `implementacion-3-castro-2025-bmssp-original/` | C++20, CMake |
| `implementacin-4-Breaking the sorting barrier/BMSSP` | Visual Studio 2022 (o MSVC con soporte C++17/20) |
| `implementacion-1-lewis-2023-dijkstra-heaps/` | g++ 7+ (C++17), Python 3 + `pandas` + `matplotlib` (solo para graficar) |
| `implementacion-2-rap2363-ssps/` | Rust (`rustup`, edición 2021+), Cargo |
| `implementacion-2-ln-bmssp-evaluation/` | Python 3.10+, Jupyter, dependencias en `pyproject.toml` |

## Cómo compilar y ejecutar cada módulo

### Aporte propio y BMSSP original (Castro et al.)

```bash
cd aporte-modificacion-codigo-bmssp   # o implementacion-3-castro-2025-bmssp-original
cmake -B build
cd build
cmake --build .
./tests
./benchmarks
```

### Traducción propia de BMSSP (verificación de correctitud)

Abrir `implementacin-4-Breaking the sorting barrier/BMSSP/BMSSP.sln` en Visual Studio 2022, compilar en modo *Release* y ejecutar `main.cpp`.

### Comparación de estructuras de datos (Lewis 2023)

```bash
cd implementacion-1-lewis-2023-dijkstra-heaps
g++ -O2 -std=c++17 -I include src/test_correctness.cpp -o test_correctness
./test_correctness      # debe imprimir "TODOS LOS TESTS PASARON"
g++ -O2 -std=c++17 -I include src/benchmark.cpp -o benchmark
./benchmark > results/benchmark_results.csv
python3 scripts/plot_results.py results/benchmark_results.csv
```

### Grafo real de Washington D.C. (Rust)

```bash
cd implementacion-2-rap2363-ssps
cargo run --release -- --pbf data/district-of-columbia-latest.osm.pbf --source 100 --out distances_bmssp.csv --algorithm bmssp
cargo run --release -- --pbf data/district-of-columbia-latest.osm.pbf --source 100 --out distances_dijkstra.csv --algorithm dijkstra
```

### Lightning Network (Valko et al. 2025)

```bash
cd implementacion-2-ln-bmssp-evaluation
jupyter notebook analysis.ipynb
```

## Resultados principales

El resumen completo con tablas y discusión está en el artículo (`articulo_IEEE.pdf`, Secciones VII y VIII). En síntesis:

- Dijkstra fue consistentemente más rápido que BMSSP-WC y BMSSP-Expected en todas las instancias evaluadas (grafos dispersos aleatorios, de cuadrícula y la red vial real `USA-road-t.NY`).
- La optimización propia **BMSSP-DedupPivots+EarlyStop** redujo el tiempo de ejecución de BMSSP-WC entre ~1% y ~2% según la instancia, sin alterar su complejidad asintótica.
- En el grafo real de Washington D.C., Dijkstra fue ~2.86× más rápido que BMSSP-WC, consistente con lo reportado por Valko et al. (2025) para la Lightning Network (~3-4x en sus propias pruebas).
- El montículo binario (Lewis 2023) resultó más rápido que el montículo de Fibonacci en todos los escenarios evaluados, pese a su peor complejidad teórica.

## Capturas de funcionamiento

_(Agregar aquí 1–2 capturas de pantalla: por ejemplo, la salida de `./test_correctness` mostrando "TODOS LOS TESTS PASARON", o el CSV de resultados generado por alguno de los benchmarks.)_

## Historial de commits y trabajo colaborativo

El historial de commits evidencia la participación de los cuatro integrantes en distintas partes del proyecto:

- **Sebastian Chávez** — aporte de modificación BMSSP, implementación 3 (Castro et al. original).
- **Graciela Ccansaya** — implementación 4 (traducción propia de BMSSP, Visual Studio).
- **Luz Ancasi** — implementación 1 (Lewis 2023, comparación de heaps).
- **Nohemi Cuba** — implementación 2 (Lightning Network y grafo real de Washington D.C.).

## Licencia

Proyecto académico desarrollado para el curso de Algoritmos Avanzados, UNSAAC, 2026. Las carpetas `implementacion-3-castro-2025-bmssp-original/`, `implementacion-2-rap2363-ssps/` e `implementacion-2-ln-bmssp-evaluation/` contienen código de terceros (con su propia licencia y atribución, ver cada `README.md` interno) usado como base para la experimentación; el resto del código es aporte propio del grupo.
