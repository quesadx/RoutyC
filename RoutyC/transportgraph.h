#ifndef TRANSPORTGRAPH_H
#define TRANSPORTGRAPH_H

#include <map>
#include <vector>

struct Edge {
    int destination;
    int weight;
    
    Edge(int dest, int w);
};

class TransportGraph {
public:
    TransportGraph();
    ~TransportGraph();
    
    void addStation(int id);
    void removeStation(int id);
    void addEdge(int source, int destination, int weight);
    void removeEdge(int source, int destination);
    bool hasEdge(int source, int destination);
    int getEdgeWeight(int source, int destination);
    std::vector<int> getNeighbors(int stationId);
    std::vector<int> getAllStations();
    void clear();
    
private:
    std::map<int, std::vector<Edge*>> adjacencyList;
    
    void removeEdgeInternal(int source, int destination);
};

#endif
