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

bool TransportGraph::isConnected(int source, int destination) {
    if (adjacencyList.find(source) == adjacencyList.end() || 
        adjacencyList.find(destination) == adjacencyList.end()) {
        return false;
    }
    
    if (source == destination) {
        return true;
    }
    
    std::vector<int> stations = getAllStations();
    std::map<int, bool> visited;
    for (int station : stations) {
        visited[station] = false;
    }
    
    std::vector<int> queue;
    queue.push_back(source);
    visited[source] = true;
    
    while (!queue.empty()) {
        int current = queue[0];
        queue.erase(queue.begin());
        
        if (current == destination) {
            return true;
        }
        
        std::vector<int> neighbors = getNeighbors(current);
        for (int neighbor : neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue.push_back(neighbor);
            }
        }
    }
    
    return false;
}

bool TransportGraph::isGraphFullyConnected() {
    std::vector<int> stations = getAllStations();
    if (stations.empty() || stations.size() == 1) {
        return true;
    }
    
    std::map<int, bool> visited;
    for (int station : stations) {
        visited[station] = false;
    }
    
    std::vector<int> queue;
    queue.push_back(stations[0]);
    visited[stations[0]] = true;
    int visitedCount = 1;
    
    while (!queue.empty()) {
        int current = queue[0];
        queue.erase(queue.begin());
        
        std::vector<int> neighbors = getNeighbors(current);
        for (int neighbor : neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                visitedCount++;
                queue.push_back(neighbor);
            }
        }
    }
    
    return visitedCount == (int)stations.size();
}
