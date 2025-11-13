#ifndef TRANSPORTGRAPH_H
#define TRANSPORTGRAPH_H

#include <map>
#include <vector>
#include <set>

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
    bool isConnected(int source, int destination);
    bool isGraphFullyConnected();
    void addClosure(int source, int destination);
    void removeClosure(int source, int destination);
    void clearClosures();
    bool isClosed(int source, int destination);
    std::vector<std::pair<int, int>> getClosures();
    
private:
    std::map<int, std::vector<Edge*>> adjacencyList;
    std::set<std::pair<int, int>> closedEdges;
    
    void removeEdgeInternal(int source, int destination);
    std::pair<int, int> normalizeEdge(int a, int b);
};

#endif
