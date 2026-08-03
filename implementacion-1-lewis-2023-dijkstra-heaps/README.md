# Benchmark: Dijkstra with Binary Heap, Fibonacci Heap, and BST

Implementation to reproduce the experiment from:

> Lewis, R. (2023). *A Comparison of Dijkstra's Algorithm Using Fibonacci
> Heaps, Binary Heaps, and Self-Balancing Binary Trees*. arXiv:2303.10034.

## What does this code do?

It implements three variants of Dijkstra's algorithm, each using a
different data structure for the priority queue:

1. **Binary Heap** (`include/dijkstra_binary_heap.hpp`) — uses
   `std::priority_queue` with the *lazy deletion* technique (no native
   decrease-key support, so duplicate entries are inserted and stale
   ones are discarded on extraction). Complexity: O(m log m).
2. **Fibonacci Heap** (`include/dijkstra_fibonacci_heap.hpp`) — uses a
   custom Fibonacci Heap implementation (`include/fibonacci_heap.hpp`)
   with real O(1) amortized decrease-key. Complexity: O(m + n log n).
3. **Self-Balancing Tree / BST** (`include/dijkstra_bst.hpp`) — uses
   `std::set` (red-black tree in the standard C++ implementation), with
   real decrease-key via delete+insert in O(log n). Complexity:
   O(m log n).

The main program (`src/benchmark.cpp`) generates random weighted
directed graphs — sparse and dense, of different sizes `n` — runs
the three variants on each graph, verifies that all three produce
exactly the same distances, measures the execution time of each one,
and exports everything to a CSV file.

## Project structure

```
sssp_benchmark/
├── include/
│   ├── graph.hpp                    # Graph structure (adjacency list)
│   ├── graph_generator.hpp          # Random graph generator (sparse/dense)
│   ├── fibonacci_heap.hpp           # Fibonacci Heap implementation
│   ├── dijkstra_binary_heap.hpp     # Dijkstra with binary heap
│   ├── dijkstra_fibonacci_heap.hpp  # Dijkstra with Fibonacci heap
│   ├── dijkstra_bst.hpp             # Dijkstra with BST (std::set)
│   └── verify.hpp                   # Result comparison with numerical tolerance
├── src/
│   ├── benchmark.cpp                # Main program: runs the full experiment
│   └── test_correctness.cpp         # Unit tests for correctness
├── scripts/
│   └── plot_results.py              # Generates charts from the results CSV
├── results/                         # Generated CSVs and figures are saved here
└── README.md
```

## How to build

A compiler with C++17 support is required (g++ 7+ or equivalent).

```bash
# Correctness test (recommended to run first)
g++ -O2 -std=c++17 -I include src/test_correctness.cpp -o test_correctness
./test_correctness

# Full benchmark
g++ -O2 -std=c++17 -I include src/benchmark.cpp -o benchmark
```

If `test_correctness` does not print "ALL TESTS PASSED", there is a
problem in the implementation and the benchmark results **should not**
be trusted.

## How to run the benchmark

```bash
./benchmark > results/benchmark_results.csv
```

This can take anywhere from a few seconds to several minutes, depending
on the configured `n` sizes (see below) and the speed of the machine.
Any warning about discrepancies between algorithms is printed to
`stderr`, not `stdout`, so it does not pollute the CSV.

### Adjusting the evaluated graph sizes

Inside `src/benchmark.cpp`, at the beginning of `main()`, the following
can be modified:

```cpp
std::vector<int> sparseSizes   = {100, 200, 500, 1000, 2000, 5000, 10000, 20000};
std::vector<int> denseSizesArg = {100, 200, 500, 1000, 2000};
int repetitions = 5;
```

- `sparseSizes`: `n` sizes for sparse graphs (fixed average out-degree,
  m = O(n)).
- `denseSizesArg`: `n` sizes for dense graphs (m ≈ 0.3·n²). Careful:
  these grow quadratically in the number of edges, so `n` values
  greater than ~3000-5000 can take quite a while, especially for the
  BST variant.
- `repetitions`: number of repetitions per configuration (to average
  out and reduce measurement noise). At least 5 is recommended; for
  more stable results in the final report, consider 10-20 if the total
  time allows it.

After modifying, recompile with the same command as above.

## How to generate the charts

Requires Python 3 with `pandas` and `matplotlib`:

```bash
pip install pandas matplotlib
python3 scripts/plot_results.py results/benchmark_results.csv
```

This generates:
- `results/figures/tiempo_vs_n_sparse.png`
- `results/figures/tiempo_vs_n_dense.png`
- `results/figures/resumen_tabla.csv` (table with average time and
  standard deviation per configuration, ready to paste into the report)

## Results CSV format

| Column               | Description                                                       |
|----------------------|--------------------------------------------------------------------|
| `tipo_grafo`         | `sparse` or `dense`                                                 |
| `n`                  | Number of vertices                                                  |
| `m`                  | Number of edges generated (actual, not the target)                  |
| `densidad_objetivo`  | Density parameter used to generate the graph                        |
| `semilla`            | Random seed used (for exact reproducibility)                        |
| `algoritmo`          | `binary_heap`, `fibonacci_heap`, or `bst`                            |
| `tiempo_ms`          | Measured execution time, in milliseconds                            |
| `distancia_total`    | Sum of all finite distances (quick consistency check)                |
| `correcto`           | 1 if it matches `binary_heap` (reference), 0 if there is a discrepancy |

## What to do with the results

1. Run `test_correctness` and confirm that everything passes.
2. Run `benchmark` and save the CSV.
3. Run `plot_results.py` to generate the charts.
4. Check the `correcto` column: it should be 1 in every row. If there is
   any 0, flag it before using that data in the report.
5. Share the full CSV (`results/benchmark_results.csv`) and the
   hardware/software specifications of the machine it was run on
   (section 1.2 of the experimentation template), so the experimentation
   chapter can be built with real data.

## Notes on the experimental design (relevant for the report)

- The graph is generated by first guaranteeing a random tree rooted at
  vertex 0 (the source), so that all vertices are reachable from the
  source — this is necessary for Dijkstra to produce finite distances
  to every node and for the comparison between algorithms to be
  meaningful.
- Edge weights are non-negative reals, generated uniformly in
  `[1.0, 1000.0]` by default (see `graph_generator.hpp`).
- The correctness check compares the final distances of the three
  variants with a numerical tolerance of `1e-6`, appropriate for
  floating-point arithmetic (`double`).
