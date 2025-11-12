#ifndef PATHALGORITHM_H
#define PATHALGORITHM_H

#include <vector>
#include <string>
#include "transportgraph.h"

struct PathResult {
    std::vector<int> path;
    int totalCost;
    std::string algorithmName;
    std::vector<std::string> steps;
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

#endif
