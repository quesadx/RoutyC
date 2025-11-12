#ifndef PATHALGORITHM_H
#define PATHALGORITHM_H

#include <vector>
#include <string>
#include "transportgraph.h"

struct VisualizationStep {
    std::vector<int> visitedNodes;
    std::vector<std::pair<int, int>> visitedEdges;
    std::string description;
};

struct PathResult {
    std::vector<int> path;
    int totalCost;
    std::string algorithmName;
    std::vector<std::string> steps;
    std::vector<VisualizationStep> visualSteps;
    bool found;
    
    PathResult();
};

class PathAlgorithm {
public:
    virtual ~PathAlgorithm();
    virtual PathResult findPath(TransportGraph* graph, int origin, int destination) = 0;
    virtual std::string getName() const = 0;
};

class DijkstraAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    std::string getName() const override;
};

class BFSAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    std::string getName() const override;
};

class DFSAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    std::string getName() const override;
    
private:
    bool dfsRecursive(TransportGraph* graph, int current, int destination, 
                      std::vector<bool>& visited, std::vector<int>& path, 
                      std::vector<int>& result, std::vector<std::string>& steps);
};

class PrimAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    std::string getName() const override;
};

class KruskalAlgorithm : public PathAlgorithm {
public:
    PathResult findPath(TransportGraph* graph, int origin, int destination) override;
    std::string getName() const override;
    
private:
    int findParent(std::vector<int>& parent, int node);
    void unionSets(std::vector<int>& parent, std::vector<int>& rank, int a, int b);
};

#endif
