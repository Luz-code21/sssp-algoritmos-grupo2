# Benchmark: Dijkstra con Binary Heap, Fibonacci Heap y BST

Implementación para reproducir el experimento de:

> Lewis, R. (2023). *A Comparison of Dijkstra's Algorithm Using Fibonacci
> Heaps, Binary Heaps, and Self-Balancing Binary Trees*. arXiv:2303.10034.

## ¿Qué hace este código?

Implementa tres variantes del algoritmo de Dijkstra, cada una usando una
estructura de datos distinta para la cola de prioridad:

1. **Binary Heap** (`include/dijkstra_binary_heap.hpp`) — usa
   `std::priority_queue` con la técnica de *lazy deletion* (no soporta
   decrease-key nativo, así que se insertan entradas duplicadas y se
   descartan las obsoletas al extraer). Complejidad: O(m log m).
2. **Fibonacci Heap** (`include/dijkstra_fibonacci_heap.hpp`) — usa una
   implementación propia de Fibonacci Heap (`include/fibonacci_heap.hpp`)
   con decrease-key real en O(1) amortizado. Complejidad: O(m + n log n).
3. **Árbol Autobalanceado / BST** (`include/dijkstra_bst.hpp`) — usa
   `std::set` (red-black tree en la implementación estándar de C++), con
   decrease-key real mediante borrar+insertar en O(log n). Complejidad:
   O(m log n).

El programa principal (`src/benchmark.cpp`) genera grafos dirigidos
ponderados aleatorios —dispersos y densos, de distintos tamaños `n`— corre
las tres variantes sobre cada grafo, verifica que las tres produzcan
exactamente las mismas distancias, mide el tiempo de ejecución de cada una,
y exporta todo a un archivo CSV.

## Estructura del proyecto

```
sssp_benchmark/
├── include/
│   ├── graph.hpp                    # Estructura del grafo (lista de adyacencia)
│   ├── graph_generator.hpp          # Generador de grafos aleatorios (sparse/dense)
│   ├── fibonacci_heap.hpp           # Implementación de Fibonacci Heap
│   ├── dijkstra_binary_heap.hpp     # Dijkstra con binary heap
│   ├── dijkstra_fibonacci_heap.hpp  # Dijkstra con Fibonacci heap
│   ├── dijkstra_bst.hpp             # Dijkstra con BST (std::set)
│   └── verify.hpp                   # Comparación de resultados con tolerancia numérica
├── src/
│   ├── benchmark.cpp                # Programa principal: corre el experimento completo
│   └── test_correctness.cpp         # Tests unitarios de corrección
├── scripts/
│   └── plot_results.py              # Genera gráficas a partir del CSV de resultados
├── results/                         # Aquí se guardan los CSV y figuras generadas
└── README.md
```

## Cómo compilar

Se requiere un compilador con soporte de C++17 (g++ 7+ o equivalente).

```bash
# Test de corrección (recomendado correr primero)
g++ -O2 -std=c++17 -I include src/test_correctness.cpp -o test_correctness
./test_correctness

# Benchmark completo
g++ -O2 -std=c++17 -I include src/benchmark.cpp -o benchmark
```

Si `test_correctness` no imprime "TODOS LOS TESTS PASARON", hay un problema
en la implementación y **no se debe** confiar en los resultados del
benchmark.

## Cómo ejecutar el benchmark

```bash
./benchmark > results/benchmark_results.csv
```

Esto puede tardar desde unos segundos hasta varios minutos, dependiendo de
los tamaños de `n` configurados (ver más abajo) y de la velocidad de la
máquina. Cualquier advertencia de discrepancia entre algoritmos se imprime
por `stderr`, no por `stdout`, así que no contamina el CSV.

### Ajustar los tamaños de grafo evaluados

Dentro de `src/benchmark.cpp`, al inicio de `main()`, se pueden modificar:

```cpp
std::vector<int> sparseSizes   = {100, 200, 500, 1000, 2000, 5000, 10000, 20000};
std::vector<int> denseSizesArg = {100, 200, 500, 1000, 2000};
int repetitions = 5;
```

- `sparseSizes`: tamaños de `n` para grafos dispersos (grado de salida
  promedio fijo, m = O(n)).
- `denseSizesArg`: tamaños de `n` para grafos densos (m ≈ 0.3·n²). Cuidado:
  estos crecen cuadráticamente en número de aristas, así que valores de
  `n` mayores a ~3000-5000 pueden tardar bastante, especialmente en la
  variante BST.
- `repetitions`: número de repeticiones por configuración (para promediar
  y reducir el ruido de medición). Se recomienda al menos 5; para resultados
  más estables en el informe final, considerar 10-20 si el tiempo total lo
  permite.

Después de modificar, recompilar con el mismo comando de arriba.

## Cómo generar las gráficas

Requiere Python 3 con `pandas` y `matplotlib`:

```bash
pip install pandas matplotlib
python3 scripts/plot_results.py results/benchmark_results.csv
```

Esto genera:
- `results/figures/tiempo_vs_n_sparse.png`
- `results/figures/tiempo_vs_n_dense.png`
- `results/figures/resumen_tabla.csv` (tabla con tiempo promedio y
  desviación estándar por configuración, lista para pegar en el informe)

## Formato del CSV de resultados

| Columna             | Descripción                                                       |
|----------------------|--------------------------------------------------------------------|
| `tipo_grafo`         | `sparse` o `dense`                                                  |
| `n`                  | Número de vértices                                                  |
| `m`                  | Número de aristas generadas (real, no el objetivo)                  |
| `densidad_objetivo`  | Parámetro de densidad usado para generar el grafo                   |
| `semilla`            | Semilla aleatoria usada (para reproducibilidad exacta)               |
| `algoritmo`          | `binary_heap`, `fibonacci_heap`, o `bst`                             |
| `tiempo_ms`          | Tiempo de ejecución medido, en milisegundos                          |
| `distancia_total`    | Suma de todas las distancias finitas (chequeo rápido de consistencia)|
| `correcto`           | 1 si coincide con `binary_heap` (referencia), 0 si hay discrepancia   |

## Qué hacer con los resultados

1. Correr `test_correctness` y confirmar que todo pasa.
2. Correr `benchmark` y guardar el CSV.
3. Correr `plot_results.py` para generar las gráficas.
4. Revisar la columna `correcto`: debe ser 1 en todas las filas. Si hay
   algún 0, avisar antes de usar esos datos en el informe.
5. Compartir el CSV completo (`results/benchmark_results.csv`) y las
   especificaciones de hardware/software de la máquina donde se corrió
   (sección 1.2 de la plantilla de experimentación), para poder construir
   el capítulo de experimentación con datos reales.

## Notas sobre el diseño experimental (relevantes para el informe)

- El grafo se genera garantizando primero un árbol aleatorio con raíz en
  el vértice 0 (la fuente), de modo que todos los vértices sean alcanzables
  desde la fuente — esto es necesario para que Dijkstra produzca distancias
  finitas a todos los nodos y la comparación entre algoritmos sea
  significativa.
- Los pesos de las aristas son reales no negativos, generados uniformemente
  en `[1.0, 1000.0]` por defecto (ver `graph_generator.hpp`).
- La verificación de corrección compara las distancias finales de las tres
  variantes con una tolerancia numérica de `1e-6`, apropiada para
  aritmética de punto flotante (`double`).
