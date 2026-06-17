#include "bmssp.h"
#include "basecase.h"
#include "Pivots.h"
#include "LevelQueue.h"
#include "Params.h"
#include "Set.h"
#include "Util.h"
#include <cmath>
#include <vector>
#include <algorithm>

namespace bmssp {

    // =========================================================================
    // bmsspCore
    // Traduccion de func bmssp(g, level, B, S, db, pred, opt) en bmssp.go
    // Nucleo recursivo del algoritmo O(m log^(2/3) n).
    // =========================================================================
    double bmsspCore(
        const Graph& g,
        int level,
        double B,
        const Set& S,
        std::vector<double>& db,
        std::vector<int>& pred,
        const Options& opt,
        Set& out_U)
    {
        // ---- Caso base: Dijkstra acotado ------------------------------------
        if (level == 0) {
            auto [Bprime, U] = baseCase(g, B, S, db, pred, opt);
            out_U = U;
            return Bprime;
        }

        // ---- Paso 1: Encontrar pivotes P y conjunto tocado W ----------------
        Set out_P, out_W;
        findPivots(g, B, S, db, pred, opt, out_P, out_W);

        // ---- Paso 2: Inicializar LevelQueue D con pivotes -------------------
        LevelQueue D(B);
        for (int v : out_P) {
            D.insert(v, db[v]);
        }

        // ---- Paso 3: Inicializar U con W ------------------------------------
        Set U;
        for (int v : out_W) {
            U.add(v);
        }

        double Bprime = B;

        // Limite de procesamiento: K^2 * 2^(level*T)
        int limit = opt.K * opt.K * intPow2(level * opt.T);

        // ---- Paso 4: Bucle principal ----------------------------------------
        while (U.size() < limit && D.nonEmpty()) {

            double Bi = 0.0;
            std::vector<int> Si = D.pull(Bi);
            if (Si.empty()) break;

            double BiPrime = Bi;
            Set Ui;

            if (level - 1 == 0) {
                // Nivel base: cada elemento de Si se procesa como singleton
                BiPrime = Bi;
                for (int v : Si) {
                    Set singleton;
                    singleton.add(v);
                    auto [bps, Uv] = baseCase(g, Bi, singleton, db, pred, opt);
                    if (bps < BiPrime) BiPrime = bps;
                    for (int x : Uv) Ui.add(x);
                }
            } else {
                // Llamada recursiva con el conjunto Si
                Set SiSet;
                for (int v : Si) SiSet.add(v);
                BiPrime = bmsspCore(g, level - 1, Bi, SiSet, db, pred, opt, Ui);
            }

            // Agregar vertices completados a U
            for (int v : Ui) U.add(v);

            // ---- Paso 5: Relajacion desde vertices completados --------------
            std::vector<KV> candidates;
            for (int u : Ui) {
                double du = db[u];
                for (const auto& e : g.neighbors(u)) {
                    double cand = du + e.w;
                    if (cand <= db[e.to]) {
                        db[e.to]   = cand;
                        pred[e.to] = u;

                        if (cand < B) {
                            // Dentro del limite actual: agregar a la cola
                            D.insert(e.to, cand);
                        } else if (cand >= BiPrime && cand < Bi) {
                            // En la banda [B'i, Bi): candidato para insercion batch
                            candidates.push_back({ e.to, cand });
                        }
                        // Mas alla de Bi: se procesara en iteraciones futuras
                    }
                }
            }

            // Insercion batch de candidatos
            D.batchPrepend(candidates);

            // Actualizar limite mas ajustado
            if (BiPrime < Bprime) Bprime = BiPrime;

            // ---- Paso 6: Absorber vertices certificados de W ----------------
            // Vertices en W con distancia < B' ya son optimos
            for (int x : out_W) {
                if (db[x] < Bprime) U.add(x);
            }
        }

        out_U = U;
        return Bprime;
    }

    // =========================================================================
    // SSSP
    // Traduccion de func SSSP(g *Graph, s int, opts ...Option) en bmssp.go
    // =========================================================================
    std::pair<std::vector<double>, std::vector<int>> SSSP(
        const Graph& g, int source, const Options* custom_opts)
    {
        int n = g.numVertices();

        // Resolver opciones
        Options opt = Options::defaultOptions(n);
        if (custom_opts != nullptr) {
            opt = *custom_opts;
        }

        // Inicializar distancias y predecesores
        std::vector<double> db(n, Inf);
        std::vector<int>    pred(n, -1);
        db[source] = 0.0;

        // Numero de niveles de recursion: l = ceil(log(n) / T)
        double logn = std::log(static_cast<double>(std::max(2, n)));
        int l = static_cast<int>(std::ceil(logn / static_cast<double>(opt.T)));

        // Frontera inicial S = {source}, limite B = Inf
        Set initialFrontier;
        initialFrontier.add(source);

        Set out_U;
        bmsspCore(g, l, Inf, initialFrontier, db, pred, opt, out_U);

        return { db, pred };
    }

} // namespace bmssp
