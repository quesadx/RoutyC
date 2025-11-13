#include "transportgraph.h"

using namespace std;

Edge::Edge(int dest, int w) : destination(dest), weight(w) {
}

TransportGraph::TransportGraph() {
}

TransportGraph::~TransportGraph() {
    clear();
}

void TransportGraph::addStation(int id) {
    if (connections.find(id) == connections.end()) {
        connections[id] = vector<Edge*>();
    }
}

void TransportGraph::removeStation(int id) {
    if (connections.find(id) == connections.end()) {
        return;
    }
    
    for (Edge* edge : connections[id]) {
        delete edge;
    }
    connections.erase(id);
    
    for (auto& pair : connections) {
        vector<Edge*>& edges = pair.second;
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

void TransportGraph::addRoute(int source, int destination, int weight) {
    addStation(source);
    addStation(destination);
    
    removeOneWayRoute(source, destination);
    removeOneWayRoute(destination, source);
    
    connections[source].push_back(new Edge(destination, weight));
    connections[destination].push_back(new Edge(source, weight));
}

void TransportGraph::removeRoute(int source, int destination) {
    removeOneWayRoute(source, destination);
    removeOneWayRoute(destination, source);
}

void TransportGraph::removeOneWayRoute(int source, int destination) {
    if (connections.find(source) == connections.end()) {
        return;
    }
    
    vector<Edge*>& edges = connections[source];
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

bool TransportGraph::hasRoute(int source, int destination) {
    if (connections.find(source) == connections.end()) {
        return false;
    }
    
    for (Edge* edge : connections[source]) {
        if (edge->destination == destination) {
            return true;
        }
    }
    return false;
}

int TransportGraph::getRouteTime(int source, int destination) {
    if (connections.find(source) == connections.end()) {
        return -1;
    }
    
    for (Edge* edge : connections[source]) {
        if (edge->destination == destination) {
            return edge->weight;
        }
    }
    return -1;
}

vector<int> TransportGraph::getConnectedStations(int stationId) {
    vector<int> neighbors;
    if (connections.find(stationId) != connections.end()) {
        for (Edge* edge : connections[stationId]) {
            if (!isRouteBlocked(stationId, edge->destination)) {
                neighbors.push_back(edge->destination);
            }
        }
    }
    return neighbors;
}

vector<int> TransportGraph::getAllConnectedStations(int stationId) {
    vector<int> neighbors;
    if (connections.find(stationId) != connections.end()) {
        for (Edge* edge : connections[stationId]) {
            neighbors.push_back(edge->destination);
        }
    }
    return neighbors;
}

vector<int> TransportGraph::getAllStations() {
    vector<int> stations;
    for (const auto& pair : connections) {
        stations.push_back(pair.first);
    }
    return stations;
}

void TransportGraph::clear() {
    for (auto& pair : connections) {
        for (Edge* edge : pair.second) {
            delete edge;
        }
    }
    connections.clear();
}

bool TransportGraph::canReach(int source, int destination) {
    if (connections.find(source) == connections.end() || 
        connections.find(destination) == connections.end()) {
        return false;
    }
    
    if (source == destination) {
        return true;
    }
    
    vector<int> stations = getAllStations();
    map<int, bool> visited;
    for (int station : stations) {
        visited[station] = false;
    }
    
    vector<int> queue;
    queue.push_back(source);
    visited[source] = true;
    
    while (!queue.empty()) {
        int current = queue[0];
        queue.erase(queue.begin());
        
        if (current == destination) {
            return true;
        }
        
        vector<int> neighbors = getConnectedStations(current);
        for (int neighbor : neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue.push_back(neighbor);
            }
        }
    }
    
    return false;
}

bool TransportGraph::isFullyConnected() {
    vector<int> stations = getAllStations();
    if (stations.empty() || stations.size() == 1) {
        return true;
    }
    
    map<int, bool> visited;
    for (int station : stations) {
        visited[station] = false;
    }
    
    vector<int> queue;
    queue.push_back(stations[0]);
    visited[stations[0]] = true;
    int visitedCount = 1;
    
    while (!queue.empty()) {
        int current = queue[0];
        queue.erase(queue.begin());
        
        vector<int> neighbors = getConnectedStations(current);
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

pair<int, int> TransportGraph::makeRoutePair(int a, int b) {
    if (a > b) {
        return {b, a};
    }
    return {a, b};
}

void TransportGraph::blockRoute(int source, int destination) {
    blockedRoutes.insert(makeRoutePair(source, destination));
}

void TransportGraph::unblockRoute(int source, int destination) {
    blockedRoutes.erase(makeRoutePair(source, destination));
}

void TransportGraph::clearBlockedRoutes() {
    blockedRoutes.clear();
}

bool TransportGraph::isRouteBlocked(int source, int destination) {
    return blockedRoutes.find(makeRoutePair(source, destination)) != blockedRoutes.end();
}

vector<pair<int, int>> TransportGraph::getBlockedRoutes() {
    vector<pair<int, int>> result;
    for (const auto& edge : blockedRoutes) {
        result.push_back(edge);
    }
    return result;
}
