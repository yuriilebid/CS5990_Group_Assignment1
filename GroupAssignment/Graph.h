#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <sstream>

using namespace std;

class Graph {
public:
    unordered_map<int, vector<int>> adjList;

    void addEdge(int u, int v) {
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    Graph(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        string line;
        // Skip the header line
        getline(file, line);

        int u, v;
        while (getline(file, line)) {
            stringstream ss(line);
            string item;
            vector<int> values;

            // Parse each line to extract u and v
            while (getline(ss, item, ',')) {
                values.push_back(stoi(item));
            }

            if (values.size() == 2) {
                u = values[0];
                v = values[1];
                this->addEdge(u, v);
            }
        }
        file.close();
    }

    unordered_set<int> getLargestComponent() {
        unordered_set<int> visited;
        unordered_set<int> largestComponent;

        for (auto& node : adjList) {
            if (visited.count(node.first) == 0) {
                unordered_set<int> component = bfs(node.first, visited);
                if (component.size() > largestComponent.size()) {
                    largestComponent = component;
                }
            }
        }
        return largestComponent;
    }

private:
    unordered_set<int> bfs(int start, unordered_set<int>& visited) {
        unordered_set<int> component;
        queue<int> q;
        q.push(start);
        visited.insert(start);

        while (!q.empty()) {
            int node = q.front();
            q.pop();
            component.insert(node);

            for (int neighbor : adjList[node]) {
                if (visited.count(neighbor) == 0) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }
        return component;
    }
};

