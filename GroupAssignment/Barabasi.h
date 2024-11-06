#ifndef BARABASI_H
#define BARABASI_H

#include <unordered_map>
#include <vector>
#include <random>
#include <unordered_set>

using namespace std;


class BarabasiAlbert {
    public:
        BarabasiAlbert(int n, int m) : n(n), m(m), gen(rd()) {
            generateNetwork();
        }

        const unordered_map<int, vector<int>>& getNetwork() const { return adjList; }

    private:
        int n, m;
        unordered_map<int, vector<int>> adjList;
        vector<int> nodes;
        random_device rd;
        mt19937 gen;

        void generateNetwork() {
            for (int i = 0; i < m; i++) {
                adjList[i] = {};
                for (int j = 0; j < i; j++) {
                    addEdge(i, j);
                }
            }

            for (int i = m; i < n; i++) {
                unordered_set<int> targets = selectTargets(i);
                adjList[i] = {};
                for (int target : targets) {
                    addEdge(i, target);
                }
            }
        }

        unordered_set<int> selectTargets(int newNode) {
            unordered_set<int> targets;
            while (targets.size() < m) {
                int node = nodes[randomIndex()];
                if (targets.find(node) == targets.end()) {
                    targets.insert(node);
                }
            }
            return targets;
        }

        int randomIndex() {
            uniform_int_distribution<> dist(0, nodes.size() - 1);
            return dist(gen);
        }

        void addEdge(int u, int v) {
            adjList[u].push_back(v);
            adjList[v].push_back(u);
            nodes.push_back(u);
            nodes.push_back(v);
        }
};



#endif //BARABASI_H
