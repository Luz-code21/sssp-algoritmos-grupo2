"""
plot_results.py

Genera gráficas a partir de results/benchmark_results.csv, comparando las
tres variantes de Dijkstra (binary heap, Fibonacci heap, BST) en función
del número de vértices (n), separadas por tipo de grafo (sparse / dense).

Requiere: pandas, matplotlib
    pip install pandas matplotlib

Uso:
    python3 plot_results.py [ruta_al_csv]

Si no se especifica ruta, se asume "results/benchmark_results.csv".
"""

import sys
import os
import pandas as pd
import matplotlib.pyplot as plt


def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else "results/benchmark_results.csv"

    if not os.path.exists(csv_path):
        print(f"No se encontró el archivo: {csv_path}")
        sys.exit(1)

    df = pd.read_csv(csv_path)

    # Promediar tiempos por (tipo_grafo, n, algoritmo) sobre las repeticiones.
    grouped = df.groupby(["tipo_grafo", "n", "algoritmo"], as_index=False).agg(
        tiempo_ms_promedio=("tiempo_ms", "mean"),
        tiempo_ms_std=("tiempo_ms", "std"),
    )

    os.makedirs("results/figures", exist_ok=True)

    algoritmos = ["binary_heap", "fibonacci_heap", "bst"]
    etiquetas = {
        "binary_heap": "Binary Heap",
        "fibonacci_heap": "Fibonacci Heap",
        "bst": "Árbol Autobalanceado (BST)",
    }
    colores = {
        "binary_heap": "#1f77b4",
        "fibonacci_heap": "#d62728",
        "bst": "#2ca02c",
    }
    marcadores = {
        "binary_heap": "o",
        "fibonacci_heap": "s",
        "bst": "^",
    }

    for tipo_grafo in grouped["tipo_grafo"].unique():
        sub = grouped[grouped["tipo_grafo"] == tipo_grafo]

        plt.figure(figsize=(8, 6))
        for algo in algoritmos:
            data_algo = sub[sub["algoritmo"] == algo].sort_values("n")
            if data_algo.empty:
                continue
            plt.errorbar(
                data_algo["n"],
                data_algo["tiempo_ms_promedio"],
                yerr=data_algo["tiempo_ms_std"],
                label=etiquetas[algo],
                color=colores[algo],
                marker=marcadores[algo],
                capsize=3,
            )

        plt.xscale("log")
        plt.yscale("log")
        plt.xlabel("Número de vértices (n)")
        plt.ylabel("Tiempo de ejecución promedio (ms)")
        titulo = "Grafos dispersos" if tipo_grafo == "sparse" else "Grafos densos"
        plt.title(f"Comparación de variantes de Dijkstra — {titulo}")
        plt.legend()
        plt.grid(True, which="both", linestyle="--", alpha=0.5)
        plt.tight_layout()

        out_path = f"results/figures/tiempo_vs_n_{tipo_grafo}.png"
        plt.savefig(out_path, dpi=150)
        plt.show()
        print(f"Figura guardada en: {out_path}")

    # Tabla resumen en texto (útil para pegar directamente en el informe)
    summary_path = "results/figures/resumen_tabla.csv"
    grouped.to_csv(summary_path, index=False)
    print(f"Tabla resumen guardada en: {summary_path}")


if __name__ == "__main__":
    main()
