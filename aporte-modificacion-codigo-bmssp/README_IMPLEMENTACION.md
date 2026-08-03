# Implementation 2: BMSSP -
Castro et al. (2025)

This folder contains the implementation used to analyze the BMSSP algorithm applied to the Single-Source Shortest Paths (SSSP) problem.

## Paper worked on

**Implementation and Experimental Analysis of the Duan et al. (2025) Algorithm for Single-Source Shortest Paths**

The paper compares the BMSSP algorithm with Dijkstra using different test graphs.

## Algorithms included

- Dijkstra
- BMSSP-WC
- BMSSP-EXPECTED

## Contribution implemented

An internal optimization was added on top of BMSSP-WC called:

**BMSSP-DedupPivots + EarlyStop**

The modification was made in:

```text
include/bmssp-worst-case.hpp
