#ifndef WATTZ_STROGATZ_H
#define WATTZ_STROGATZ_H

#include <cmath>
#include <random>
#include <unordered_map>
#include <map>
#include <vector>

using namespace std;

class WattsStrogatz {
public:
    WattsStrogatz(int n, int k, double beta) : n(n), k(k), beta(beta), gen(rd()) {
        generateRingLattice();
        rewireEdges();
    }

    const unordered_map<int, vector<int>>& getNetwork() const { return adjList; }

private:
    int n, k;
    double beta;
    unordered_map<int, vector<int>> adjList;
    random_device rd;
    mt19937 gen;

    void generateRingLattice() {
        for (int i = 0; i < n; i++) {
            for (int j = 1; j <= k / 2; j++) {
                int neighbor = (i + j) % n;
                adjList[i].push_back(neighbor);
                adjList[neighbor].push_back(i);
            }
        }
    }

    void rewireEdges() {
        uniform_real_distribution<> dist(0, 1);
        for (int i = 0; i < n; i++) {
            for (int j = 1; j <= k / 2; j++) {
                if (dist(gen) < beta) {
                    int neighbor = (i + j) % n;
                    adjList[i].erase(remove(adjList[i].begin(), adjList[i].end(), neighbor), adjList[i].end());
                    int newNeighbor = randomNode(i);
                    adjList[i].push_back(newNeighbor);
                    adjList[newNeighbor].push_back(i);
                }
            }
        }
    }

    int randomNode(int exclude) {
        uniform_int_distribution<> dist(0, n - 1);
        int node;
        do {
            node = dist(gen);
        } while (node == exclude || count(adjList[exclude].begin(), adjList[exclude].end(), node));
        return node;
    }
};




#endif //WATTZ_STROGATZ_H
