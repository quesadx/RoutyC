#ifndef PATHALGORITHM_H
#define PATHALGORITHM_H

#include <vector>
#include <string>
#include "TransportGraph.h"

using namespace std;

struct VisualizationStep {
    vector<int> visitedNodes;
    vector<pair<int, int>> visitedEdges;
    string description;
};

struct PathResult {
    vector<int> path;
    int totalCost;
    string algorithmName;
    vector<string> steps;
    vector<VisualizationStep> visualSteps;
    bool found;
    
    PathResult();
};

class PathAlgorithm {
public:
    virtual ~PathAlgorithm();
    virtual PathResult findPath(TransportGraph* graph, int origin, int destination) = 0;
    virtual string getName() const = 0;
};

class DijkstraAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    string getName() const override;
};

class BFSAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    string getName() const override;
};

class DFSAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    string getName() const override;
    
private:
    bool dfsRecursive(TransportGraph* graph, int current, int destination, 
                      vector<bool>& visited, vector<int>& path, 
                      vector<int>& result, vector<string>& steps,
                      vector<VisualizationStep>& visualSteps);
};

class PrimAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    string getName() const override;
};

class KruskalAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    string getName() const override;
    
private:
    int findParent(vector<int>& parent, int node);
    void unionSets(vector<int>& parent, vector<int>& rank, int a, int b);
};

class FloydWarshallAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    string getName() const override;
};

#endif
