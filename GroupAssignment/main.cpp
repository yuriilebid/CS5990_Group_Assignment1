#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <cmath>
#include <thread>
#include <future>
#include <fstream>
#include <chrono>
#include "Graph.h"
#include "Barabasi.h"       // Include Barabási-Albert model implementation
#include "wattz_strogatz.h"  // Include Watts-Strogatz model implementation

using namespace std;
using namespace chrono;

ofstream logFile("log.txt");  // Файл для записи логов

// Функция для вывода в консоль и запись в файл одновременно
void logMessage(const string& message) {
    cout << message << endl;
    logFile << message << endl;
}

double averageDegree(const unordered_map<int, vector<int>>& adjList) {
    logMessage("[INFO] Calculating average degree...");
    double degreeSum = 0;
    for (const auto& node : adjList) {
        degreeSum += node.second.size();
    }
    double avgDegree = degreeSum / adjList.size();
    logMessage("[INFO] Average degree calculated: " + to_string(avgDegree));
    return avgDegree;
}

double calculatePartialPathLength(const unordered_map<int, vector<int>>& adjList, const vector<int>& nodes, int threadId) {
    auto start = high_resolution_clock::now();
    double totalPathLength = 0;
    int reachablePairs = 0;
    int processedNodes = 0;

    for (int startNode : nodes) {
        unordered_map<int, int> distances;
        queue<int> q;
        q.push(startNode);
        distances[startNode] = 0;

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            for (int neighbor : adjList.at(node)) {
                if (distances.count(neighbor) == 0) {
                    distances[neighbor] = distances[node] + 1;
                    q.push(neighbor);
                }
            }
        }

        for (const auto& distance : distances) {
            if (distance.first != startNode) {
                totalPathLength += distance.second;
                reachablePairs++;
            }
        }

        processedNodes++;
        if (processedNodes % 100 == 0) {
            logMessage("[Thread " + to_string(threadId) + "] Processed nodes for path length: " + to_string(processedNodes));
        }
    }

    auto end = high_resolution_clock::now();
    double avgPathLength = reachablePairs > 0 ? totalPathLength / reachablePairs : 0;
    logMessage("[Thread " + to_string(threadId) + "] Average path length calculated: " + to_string(avgPathLength) +
               " | Time taken: " + to_string(duration_cast<milliseconds>(end - start).count()) + " ms");
    return avgPathLength;
}

double averagePathLength(const unordered_map<int, vector<int>>& adjList) {
    logMessage("[INFO] Calculating average path length in parallel...");
    vector<int> nodes;
    for (const auto& node : adjList) {
        nodes.push_back(node.first);
    }

    size_t numThreads = 6;
    vector<vector<int>> partitions(numThreads);
    for (size_t i = 0; i < nodes.size(); ++i) {
        partitions[i % numThreads].push_back(nodes[i]);
    }

    vector<future<double>> futures;
    for (size_t i = 0; i < numThreads; ++i) {
        futures.push_back(async(launch::async, calculatePartialPathLength, ref(adjList), partitions[i], i + 1));
    }

    double totalPathLength = 0;
    for (auto& future : futures) {
        totalPathLength += future.get();
    }

    double avgPathLength = totalPathLength / numThreads;
    logMessage("[INFO] Average path length calculated: " + to_string(avgPathLength));
    return avgPathLength;
}

double calculatePartialClustering(const unordered_map<int, vector<int>>& adjList, const vector<int>& nodes, int threadId) {
    auto start = high_resolution_clock::now();
    double totalClustering = 0;
    int processedNodes = 0;

    for (int node : nodes) {
        int degree = adjList.at(node).size();
        if (degree < 2) continue;

        int actualEdges = 0;
        for (int neighbor : adjList.at(node)) {
            for (int other : adjList.at(node)) {
                if (neighbor != other &&
                    adjList.find(neighbor) != adjList.end() &&
                    find(adjList.at(neighbor).begin(), adjList.at(neighbor).end(), other) != adjList.at(neighbor).end()) {
                    actualEdges++;
                }
            }
        }

        double clustering = static_cast<double>(actualEdges) / (degree * (degree - 1));
        totalClustering += clustering;

        processedNodes++;
        if (processedNodes % 100 == 0) {
            logMessage("[Thread " + to_string(threadId) + "] Processed nodes for clustering coefficient: " + to_string(processedNodes));
        }
    }

    auto end = high_resolution_clock::now();
    double avgClustering = totalClustering / nodes.size();
    logMessage("[Thread " + to_string(threadId) + "] Clustering coefficient calculated: " + to_string(avgClustering) +
               " | Time taken: " + to_string(duration_cast<milliseconds>(end - start).count()) + " ms");
    return avgClustering;
}

double clusteringCoefficient(const unordered_map<int, vector<int>>& adjList) {
    logMessage("[INFO] Calculating clustering coefficient in parallel...");
    vector<int> nodes;
    for (const auto& node : adjList) {
        nodes.push_back(node.first);
    }

    size_t numThreads = 6;
    vector<vector<int>> partitions(numThreads);
    for (size_t i = 0; i < nodes.size(); ++i) {
        partitions[i % numThreads].push_back(nodes[i]);
    }

    vector<future<double>> futures;
    for (size_t i = 0; i < numThreads; ++i) {
        futures.push_back(async(launch::async, calculatePartialClustering, ref(adjList), partitions[i], i + 1));
    }

    double totalClustering = 0;
    for (auto& future : futures) {
        totalClustering += future.get();
    }

    double avgClustering = totalClustering / numThreads;
    logMessage("[INFO] Clustering coefficient calculated: " + to_string(avgClustering));
    return avgClustering;
}

int main() {
    logMessage("[INFO] Initializing and parsing Twitch data...");
    Graph twitchGraph = Graph("/Users/yuriilebid/CLionProjects/GroupAssignment/large_twitch_edges.csv");

    logMessage("[INFO] Extracting largest connected component...");
    unordered_set<int> largestComponent = twitchGraph.getLargestComponent();
    logMessage("[INFO] Largest component size: " + to_string(largestComponent.size()));

    int networkSize = largestComponent.size();
    int avgDegree = averageDegree(twitchGraph.adjList);

    logMessage("[INFO] Creating Watts-Strogatz and Barabási-Albert models...");
    WattsStrogatz wsModel(networkSize, avgDegree, 0.1);  // Adjust beta as needed
    BarabasiAlbert baModel(networkSize, avgDegree / 2);

    logMessage("[INFO] Calculating metrics for Watts-Strogatz model...");
    double wsAvgDegree = averageDegree(wsModel.getNetwork());
    double wsClustering = clusteringCoefficient(wsModel.getNetwork());
    double wsPathLength = averagePathLength(wsModel.getNetwork());

    logMessage("[INFO] Calculating metrics for Barabási-Albert model...");
    double baAvgDegree = averageDegree(baModel.getNetwork());
    double baClustering = clusteringCoefficient(baModel.getNetwork());
    double baPathLength = averagePathLength(baModel.getNetwork());

    logMessage("\nComparison:");
    logMessage("Original Network - Size: " + to_string(networkSize) + ", Avg Degree: " + to_string(avgDegree) +
               ", Avg Path Length: " + to_string(averagePathLength(twitchGraph.adjList)) +
               ", Clustering Coefficient: " + to_string(clusteringCoefficient(twitchGraph.adjList)));

    logMessage("Watts-Strogatz - Avg Degree: " + to_string(wsAvgDegree) +
               ", Avg Path Length: " + to_string(wsPathLength) +
               ", Clustering Coefficient: " + to_string(wsClustering));

    logMessage("Barabási-Albert - Avg Degree: " + to_string(baAvgDegree) +
               ", Avg Path Length: " + to_string(baPathLength) +
               ", Clustering Coefficient: " + to_string(baClustering));

    logMessage("[INFO] Program completed successfully.");
    logFile.close();
    return 0;
}
