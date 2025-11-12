#include "transportgraph.h"

Edge::Edge(int dest, int w) : destination(dest), weight(w) {
}

TransportGraph::TransportGraph() {
}

TransportGraph::~TransportGraph() {
    clear();
}

void TransportGraph::addStation(int id) {
    if (adjacencyList.find(id) == adjacencyList.end()) {
        adjacencyList[id] = std::vector<Edge*>();
    }
}

void TransportGraph::removeStation(int id) {
    if (adjacencyList.find(id) == adjacencyList.end()) {
        return;
    }
    
    for (Edge* edge : adjacencyList[id]) {
        delete edge;
    }
    adjacencyList.erase(id);
    
    for (auto& pair : adjacencyList) {
        std::vector<Edge*>& edges = pair.second;
        for (auto it = edges.begin(); it != edges.end();) {
            if ((*it)->destination == id) {
                delete *it;
                it = edges.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void TransportGraph::addEdge(int source, int destination, int weight) {
    addStation(source);
    addStation(destination);
    
    removeEdgeInternal(source, destination);
    removeEdgeInternal(destination, source);
    
    adjacencyList[source].push_back(new Edge(destination, weight));
    adjacencyList[destination].push_back(new Edge(source, weight));
}

void TransportGraph::removeEdge(int source, int destination) {
    removeEdgeInternal(source, destination);
    removeEdgeInternal(destination, source);
}

void TransportGraph::removeEdgeInternal(int source, int destination) {
    if (adjacencyList.find(source) == adjacencyList.end()) {
        return;
    }
    
    std::vector<Edge*>& edges = adjacencyList[source];
    for (auto it = edges.begin(); it != edges.end();) {
        if ((*it)->destination == destination) {
            delete *it;
            it = edges.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

bool TransportGraph::hasEdge(int source, int destination) {
    if (adjacencyList.find(source) == adjacencyList.end()) {
        return false;
    }
    
    for (Edge* edge : adjacencyList[source]) {
        if (edge->destination == destination) {
            return true;
        }
    }
    return false;
}

int TransportGraph::getEdgeWeight(int source, int destination) {
    if (adjacencyList.find(source) == adjacencyList.end()) {
        return -1;
    }
    
    for (Edge* edge : adjacencyList[source]) {
        if (edge->destination == destination) {
            return edge->weight;
        }
    }
    return -1;
}

std::vector<int> TransportGraph::getNeighbors(int stationId) {
    std::vector<int> neighbors;
    if (adjacencyList.find(stationId) != adjacencyList.end()) {
        for (Edge* edge : adjacencyList[stationId]) {
            neighbors.push_back(edge->destination);
        }
    }
    return neighbors;
}

std::vector<int> TransportGraph::getAllStations() {
    std::vector<int> stations;
    for (const auto& pair : adjacencyList) {
        stations.push_back(pair.first);
    }
    return stations;
}

void TransportGraph::clear() {
    for (auto& pair : adjacencyList) {
        for (Edge* edge : pair.second) {
            delete edge;
        }
    }
    adjacencyList.clear();
}
