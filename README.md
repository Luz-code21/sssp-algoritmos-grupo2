# SSSP Algorithm Comparison: Dijkstra vs. BMSSP

**Universidad Nacional de San Antonio Abad del Cusco (UNSAAC)**
Professional School of Computer and Systems Engineering
Course: Advanced Algorithms — Group 2



## Members

| Member | Code |
|---|---|
| Luz Diana Ancasi Aymachoque | 215714 |
| Yhasmin Graciela Ccansaya Corrales | 230249 |
| Bryan Sebastian Chávez Luna | 215817 |
| Nohemi Sharit Cuba Castillo | 230250 |

## Overview

This repository documents the experimental comparison between the classic **Dijkstra** algorithm and the deterministic **BMSSP** algorithm (*Bounded Multi-Source Shortest Path*, Duan et al., STOC 2025), which breaks Dijkstra's classic sorting barrier on sparse directed graphs. It replicates and extends the benchmarks from three recent works (Castro et al. 2025, Valko et al. 2025, Lewis 2023), and proposes an original optimization on top of BMSSP: **BMSSP-DedupPivots+EarlyStop**.

Each folder in this repository corresponds to a distinct implementation or experiment, with its own detailed README. This document is the general entry point.

## Repository structure

| Folder | Content | What is it? |
|---|---|---|
| **`aporte-modificacion-codigo-bmssp/`** | C++20, header-only, based on Castro et al.'s implementation | **Original contribution of the group**: BMSSP-DedupPivots+EarlyStop optimization on top of Castro et al.'s code. See `README_IMPLEMENTACION.md` inside this folder for details on the modification. |
| **`implementacin-4-Breaking the sorting barrier/BMSSP`** | C++ (Visual Studio 2022, `.sln`) | Own, independent translation of the BMSSP algorithm (from Duan et al.'s Go reference), used to verify correctness on small graphs (Section III of the paper). |
| **`implementacion-1-lewis-2023-dijkstra-heaps/`** | C++17 | Replication of Lewis's (2023) experiment: Dijkstra with binary heap, Fibonacci heap, and self-balancing tree (BST). |
| **`implementacion-2-rap2363-ssps/`** | Rust | Third-party base repository with Dijkstra and BMSSP in Rust, used to run tests on real graphs (OSM), including the Washington D.C. graph cited in the paper. |
| **`implementacion-2-ln-bmssp-evaluation/`** | Python / Jupyter | Experimental analysis notebook associated with the BMSSP paper applied to the Lightning Network (Valko et al., 2025). |
| **`implementacion-3-castro-2025-bmssp-original/`** | C++20, header-only | Original (unmodified) implementation by Castro et al. (2025), used as the baseline code for the extended benchmark in Table 2 of the paper. |

> **Note:** the `implementacion-3-castro-2025-bmssp-original/` and `aporte-modificacion-codigo-bmssp/` folders start from the same Castro et al. code; the difference is that the latter includes the group's own modification (DedupPivots+EarlyStop). Don't confuse one with the other when reviewing the original contribution.

## Requirements and installation per module

| Module | Requirements |
|---|---|
| `aporte-modificacion-codigo-bmssp/`, `implementacion-3-castro-2025-bmssp-original/` | C++20, CMake |
| `implementacin-4-Breaking the sorting barrier/BMSSP` | Visual Studio 2022 (or MSVC with C++17/20 support) |
| `implementacion-1-lewis-2023-dijkstra-heaps/` | g++ 7+ (C++17), Python 3 + `pandas` + `matplotlib` (for plotting only) |
| `implementacion-2-rap2363-ssps/` | Rust (`rustup`, 2021+ edition), Cargo |
| `implementacion-2-ln-bmssp-evaluation/` | Python 3.10+, Jupyter, dependencies in `pyproject.toml` |

## How to build and run each module

### Own contribution and original BMSSP (Castro et al.)

```bash
cd aporte-modificacion-codigo-bmssp   # or implementacion-3-castro-2025-bmssp-original
cmake -B build
cd build
cmake --build .
./tests
./benchmarks
```

### Own translation of BMSSP

Open `implementacin-4-Breaking the sorting barrier/BMSSP/BMSSP.sln` in Visual Studio 2022, build in *Release* mode, and run `main.cpp`.

### Data structure comparison (Lewis 2023)

```bash
cd implementacion-1-lewis-2023-dijkstra-heaps
g++ -O2 -std=c++17 -I include src/test_correctness.cpp -o test_correctness
./test_correctness      # should print "ALL TESTS PASSED"
g++ -O2 -std=c++17 -I include src/benchmark.cpp -o benchmark
./benchmark > results/benchmark_results.csv
python3 scripts/plot_results.py results/benchmark_results.csv
```

### Real Washington D.C. graph (Rust)

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

## Main results

The complete summary with tables and discussion is in the paper (`articulo_IEEE.pdf`, Sections VII and VIII). In summary:

- Dijkstra was consistently faster than BMSSP-WC and BMSSP-Expected across all evaluated instances (random sparse graphs, grid graphs, and the real road network `USA-road-t.NY`).
- The original **BMSSP-DedupPivots+EarlyStop** optimization reduced BMSSP-WC's execution time by ~1% to ~2% depending on the instance, without altering its asymptotic complexity.
- On the real Washington D.C. graph, Dijkstra was ~2.86× faster than BMSSP-WC, consistent with what was reported by Valko et al. (2025) for the Lightning Network (~3-4x in their own tests).
- The binary heap (Lewis 2023) turned out to be faster than the Fibonacci heap in all evaluated scenarios, despite its worse theoretical complexity.

## Screenshots



## Commit history and collaborative work


- **Sebastian Chávez** — BMSSP modification contribution, implementation 3 (original Castro et al.).
- **Graciela Ccansaya** — implementation 4 (own translation of BMSSP, Visual Studio).
- **Luz Ancasi** — implementation 1 (Lewis 2023, heap comparison).
- **Nohemi Cuba** — implementation 2 (Lightning Network and real Washington D.C. graph).

## License

Academic project developed for the Advanced Algorithms course, UNSAAC, 2026. The `implementacion-3-castro-2025-bmssp-original/`, `implementacion-2-rap2363-ssps/`, and `implementacion-2-ln-bmssp-evaluation/` folders contain third-party code (with its own license and attribution, see each internal `README.md`) used as a base for experimentation; the rest of the code is the group's own contribution.
