#include "pathalgorithm.h"
#include <queue>
#include <map>
#include <set>
#include <limits>
#include <algorithm>

PathResult::PathResult() : totalCost(0), algorithmName(""), found(false) {
}

PathAlgorithm::~PathAlgorithm() {
}

PathResult DijkstraAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "Dijkstra";
    result.found = false;
    
    std::vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No stations in graph");
        return result;
    }
    
    std::map<int, int> distances;
    std::map<int, int> previous;
    std::set<int> unvisited;
    
    for (int station : stations) {
        distances[station] = std::numeric_limits<int>::max();
        unvisited.insert(station);
    }
    
    distances[origin] = 0;
    result.steps.push_back("Starting Dijkstra from station " + std::to_string(origin));
    
    while (!unvisited.empty()) {
        int current = -1;
        int minDist = std::numeric_limits<int>::max();
        
        for (int station : unvisited) {
            if (distances[station] < minDist) {
                minDist = distances[station];
                current = station;
            }
        }
        
        if (current == -1 || distances[current] == std::numeric_limits<int>::max()) {
            break;
        }
        
        unvisited.erase(current);
        result.steps.push_back("Visiting station " + std::to_string(current) + 
                              " (distance: " + std::to_string(distances[current]) + ")");
        
        if (current == destination) {
            result.found = true;
            break;
        }
        
        std::vector<int> neighbors = graph->getNeighbors(current);
        for (int neighbor : neighbors) {
            if (unvisited.find(neighbor) == unvisited.end()) {
                continue;
            }
            
            int weight = graph->getEdgeWeight(current, neighbor);
            int newDist = distances[current] + weight;
            
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                previous[neighbor] = current;
                result.steps.push_back("  Updated station " + std::to_string(neighbor) + 
                                      " distance to " + std::to_string(newDist));
            }
        }
    }
    
    if (result.found) {
        int current = destination;
        while (current != origin) {
            result.path.insert(result.path.begin(), current);
            if (previous.find(current) == previous.end()) {
                result.found = false;
                result.steps.push_back("Path reconstruction failed");
                return result;
            }
            current = previous[current];
        }
        result.path.insert(result.path.begin(), origin);
        result.totalCost = distances[destination];
        result.steps.push_back("Path found with total cost: " + std::to_string(result.totalCost));
    } else {
        result.steps.push_back("No path found to destination");
    }
    
    return result;
}

std::string DijkstraAlgorithm::getName() const {
    return "Dijkstra";
}

PathResult BFSAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "BFS";
    result.found = false;
    
    std::vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No stations in graph");
        return result;
    }
    
    std::map<int, bool> visited;
    std::map<int, int> previous;
    std::queue<int> queue;
    
    for (int station : stations) {
        visited[station] = false;
    }
    
    queue.push(origin);
    visited[origin] = true;
    result.steps.push_back("Starting BFS from station " + std::to_string(origin));
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        result.steps.push_back("Visiting station " + std::to_string(current));
        
        if (current == destination) {
            result.found = true;
            break;
        }
        
        std::vector<int> neighbors = graph->getNeighbors(current);
        for (int neighbor : neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                previous[neighbor] = current;
                queue.push(neighbor);
                result.steps.push_back("  Enqueued station " + std::to_string(neighbor));
            }
        }
    }
    
    if (result.found) {
        int current = destination;
        int cost = 0;
        
        while (current != origin) {
            result.path.insert(result.path.begin(), current);
            if (previous.find(current) == previous.end()) {
                result.found = false;
                result.steps.push_back("Path reconstruction failed");
                return result;
            }
            int prev = previous[current];
            cost += graph->getEdgeWeight(prev, current);
            current = prev;
        }
        result.path.insert(result.path.begin(), origin);
        result.totalCost = cost;
        result.steps.push_back("Path found with total cost: " + std::to_string(result.totalCost));
    } else {
        result.steps.push_back("No path found to destination");
    }
    
    return result;
}

std::string BFSAlgorithm::getName() const {
    return "BFS (Breadth-First Search)";
}

PathResult DFSAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "DFS";
    result.found = false;
    
    std::vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No stations in graph");
        return result;
    }
    
    std::map<int, bool> visitedMap;
    for (int station : stations) {
        visitedMap[station] = false;
    }
    
    std::vector<bool> visited(10000, false);
    std::vector<int> path;
    std::vector<int> foundPath;
    
    result.steps.push_back("Starting DFS from station " + std::to_string(origin));
    
    for (int station : stations) {
        visited[station] = visitedMap[station];
    }
    
    result.found = dfsRecursive(graph, origin, destination, visited, path, foundPath, result.steps);
    
    if (result.found) {
        result.path = foundPath;
        int cost = 0;
        for (size_t i = 0; i < result.path.size() - 1; i++) {
            cost += graph->getEdgeWeight(result.path[i], result.path[i + 1]);
        }
        result.totalCost = cost;
        result.steps.push_back("Path found with total cost: " + std::to_string(result.totalCost));
    } else {
        result.steps.push_back("No path found to destination");
    }
    
    return result;
}

bool DFSAlgorithm::dfsRecursive(TransportGraph* graph, int current, int destination,
                                std::vector<bool>& visited, std::vector<int>& path,
                                std::vector<int>& result, std::vector<std::string>& steps) {
    visited[current] = true;
    path.push_back(current);
    steps.push_back("Visiting station " + std::to_string(current));
    
    if (current == destination) {
        result = path;
        return true;
    }
    
    std::vector<int> neighbors = graph->getNeighbors(current);
    for (int neighbor : neighbors) {
        if (!visited[neighbor]) {
            if (dfsRecursive(graph, neighbor, destination, visited, path, result, steps)) {
                return true;
            }
        }
    }
    
    path.pop_back();
    steps.push_back("Backtracking from station " + std::to_string(current));
    return false;
}

std::string DFSAlgorithm::getName() const {
    return "DFS (Depth-First Search)";
}
