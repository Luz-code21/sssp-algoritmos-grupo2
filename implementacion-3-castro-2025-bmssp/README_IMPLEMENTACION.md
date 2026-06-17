# Implementación 2: BMSSP - 
Castro et al. (2025)

Esta carpeta contiene la implementación utilizada para analizar el algoritmo BMSSP aplicado al problema Single-Source Shortest Paths (SSSP).

## Paper trabajado

**Implementation and Experimental Analysis of the Duan et al. (2025) Algorithm for Single-Source Shortest Paths**

El trabajo compara el algoritmo BMSSP con Dijkstra usando diferentes grafos de prueba.

## Algoritmos incluidos

- Dijkstra
- BMSSP-WC
- BMSSP-EXPECTED

## Aporte implementado

Se agregó una optimización interna sobre BMSSP-WC denominada:

**BMSSP-DedupPivots + EarlyStop**

La modificación se realizó en:

```text
include/bmssp-worst-case.hpp
