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
    std::set<int> visited;
    
    for (int station : stations) {
        distances[station] = std::numeric_limits<int>::max();
        unvisited.insert(station);
    }
    
    distances[origin] = 0;
    result.steps.push_back("Starting Dijkstra from station " + std::to_string(origin));
    
    VisualizationStep initStep;
    initStep.visitedNodes.push_back(origin);
    initStep.description = "Starting from station " + std::to_string(origin);
    result.visualSteps.push_back(initStep);
    
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
        visited.insert(current);
        result.steps.push_back("Visiting station " + std::to_string(current) + 
                              " (distance: " + std::to_string(distances[current]) + ")");
        
        VisualizationStep step;
        for (int v : visited) {
            step.visitedNodes.push_back(v);
        }
        for (int v : visited) {
            if (previous.find(v) != previous.end()) {
                step.visitedEdges.push_back({previous[v], v});
            }
        }
        step.description = "Visiting station " + std::to_string(current) + " (distance: " + std::to_string(distances[current]) + ")";
        result.visualSteps.push_back(step);
        
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
    std::vector<int> visitedOrder;
    
    for (int station : stations) {
        visited[station] = false;
    }
    
    queue.push(origin);
    visited[origin] = true;
    visitedOrder.push_back(origin);
    result.steps.push_back("Starting BFS from station " + std::to_string(origin));
    
    VisualizationStep initStep;
    initStep.visitedNodes.push_back(origin);
    initStep.description = "Starting BFS from station " + std::to_string(origin);
    result.visualSteps.push_back(initStep);
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        result.steps.push_back("Visiting station " + std::to_string(current));
        
        VisualizationStep step;
        for (int v : visitedOrder) {
            step.visitedNodes.push_back(v);
        }
        for (int v : visitedOrder) {
            if (previous.find(v) != previous.end()) {
                step.visitedEdges.push_back({previous[v], v});
            }
        }
        step.description = "Visiting station " + std::to_string(current);
        result.visualSteps.push_back(step);
        
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
                visitedOrder.push_back(neighbor);
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
    
    VisualizationStep initStep;
    initStep.visitedNodes.push_back(origin);
    initStep.description = "Starting DFS from station " + std::to_string(origin);
    result.visualSteps.push_back(initStep);
    
    for (int station : stations) {
        visited[station] = visitedMap[station];
    }
    
    result.found = dfsRecursive(graph, origin, destination, visited, path, foundPath, result.steps, result.visualSteps);
    
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
                                std::vector<int>& result, std::vector<std::string>& steps,
                                std::vector<VisualizationStep>& visualSteps) {
    visited[current] = true;
    path.push_back(current);
    steps.push_back("Visiting station " + std::to_string(current));
    
    VisualizationStep step;
    for (int node : path) {
        step.visitedNodes.push_back(node);
    }
    for (size_t i = 1; i < path.size(); i++) {
        step.visitedEdges.push_back({path[i-1], path[i]});
    }
    step.description = "Visiting station " + std::to_string(current);
    visualSteps.push_back(step);
    
    if (current == destination) {
        result = path;
        return true;
    }
    
    std::vector<int> neighbors = graph->getNeighbors(current);
    for (int neighbor : neighbors) {
        if (!visited[neighbor]) {
            if (dfsRecursive(graph, neighbor, destination, visited, path, result, steps, visualSteps)) {
                return true;
            }
        }
    }
    
    path.pop_back();
    steps.push_back("Backtracking from station " + std::to_string(current));
    
    VisualizationStep backtrackStep;
    for (int node : path) {
        backtrackStep.visitedNodes.push_back(node);
    }
    for (size_t i = 1; i < path.size(); i++) {
        backtrackStep.visitedEdges.push_back({path[i-1], path[i]});
    }
    backtrackStep.description = "Backtracking from station " + std::to_string(current);
    visualSteps.push_back(backtrackStep);
    
    return false;
}

std::string DFSAlgorithm::getName() const {
    return "DFS (Depth-First Search)";
}

PathResult PrimAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "Prim MST";
    result.found = false;
    
    std::vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No stations in graph");
        return result;
    }
    
    std::map<int, bool> inMST;
    std::map<int, int> key;
    std::map<int, int> parent;
    
    for (int station : stations) {
        inMST[station] = false;
        key[station] = std::numeric_limits<int>::max();
    }
    
    key[origin] = 0;
    parent[origin] = -1;
    result.steps.push_back("Starting Prim's MST from station " + std::to_string(origin));
    
    VisualizationStep initStep;
    initStep.visitedNodes.push_back(origin);
    initStep.description = "Starting from station " + std::to_string(origin);
    result.visualSteps.push_back(initStep);
    
    for (size_t count = 0; count < stations.size(); count++) {
        int u = -1;
        int minKey = std::numeric_limits<int>::max();
        
        for (int station : stations) {
            if (!inMST[station] && key[station] < minKey) {
                minKey = key[station];
                u = station;
            }
        }
        
        if (u == -1) break;
        
        inMST[u] = true;
        result.steps.push_back("Added station " + std::to_string(u) + " to MST");
        
        VisualizationStep step;
        for (int station : stations) {
            if (inMST[station]) {
                step.visitedNodes.push_back(station);
            }
        }
        if (parent[u] != -1) {
            step.visitedEdges.push_back({parent[u], u});
        }
        step.description = "Added station " + std::to_string(u);
        result.visualSteps.push_back(step);
        
        std::vector<int> neighbors = graph->getNeighbors(u);
        for (int v : neighbors) {
            int weight = graph->getEdgeWeight(u, v);
            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
            }
        }
    }
    
    if (inMST[destination]) {
        result.found = true;
        int current = destination;
        while (current != -1 && parent[current] != -1) {
            result.path.insert(result.path.begin(), current);
            current = parent[current];
        }
        if (current != -1) {
            result.path.insert(result.path.begin(), current);
        }
        
        int cost = 0;
        for (size_t i = 0; i < result.path.size() - 1; i++) {
            cost += graph->getEdgeWeight(result.path[i], result.path[i + 1]);
        }
        result.totalCost = cost;
        result.steps.push_back("MST path found with cost: " + std::to_string(cost));
    } else {
        result.steps.push_back("Destination not reachable in MST");
    }
    
    return result;
}

std::string PrimAlgorithm::getName() const {
    return "Prim (MST)";
}

PathResult KruskalAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "Kruskal MST";
    result.found = false;
    
    std::vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No stations in graph");
        return result;
    }
    
    std::vector<std::pair<int, std::pair<int, int>>> edges;
    for (int station : stations) {
        std::vector<int> neighbors = graph->getNeighbors(station);
        for (int neighbor : neighbors) {
            if (station < neighbor) {
                int weight = graph->getEdgeWeight(station, neighbor);
                edges.push_back({weight, {station, neighbor}});
            }
        }
    }
    
    std::sort(edges.begin(), edges.end());
    
    std::vector<int> parent(10000);
    std::vector<int> rank(10000, 0);
    for (int station : stations) {
        parent[station] = station;
    }
    
    result.steps.push_back("Starting Kruskal's MST algorithm");
    std::vector<std::pair<int, int>> mstEdges;
    
    for (auto& edge : edges) {
        int weight = edge.first;
        int u = edge.second.first;
        int v = edge.second.second;
        
        int setU = findParent(parent, u);
        int setV = findParent(parent, v);
        
        if (setU != setV) {
            mstEdges.push_back({u, v});
            unionSets(parent, rank, setU, setV);
            result.steps.push_back("Added edge " + std::to_string(u) + "-" + 
                                  std::to_string(v) + " (weight: " + std::to_string(weight) + ")");
            
            VisualizationStep step;
            std::set<int> connectedNodes;
            for (auto& e : mstEdges) {
                connectedNodes.insert(e.first);
                connectedNodes.insert(e.second);
                step.visitedEdges.push_back(e);
            }
            for (int node : connectedNodes) {
                step.visitedNodes.push_back(node);
            }
            step.description = "Added edge " + std::to_string(u) + "-" + std::to_string(v);
            result.visualSteps.push_back(step);
        }
    }
    
    std::map<int, std::vector<int>> mstGraph;
    for (auto& edge : mstEdges) {
        mstGraph[edge.first].push_back(edge.second);
        mstGraph[edge.second].push_back(edge.first);
    }
    
    std::map<int, bool> visited;
    std::map<int, int> parentMap;
    std::queue<int> queue;
    
    queue.push(origin);
    visited[origin] = true;
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        if (current == destination) {
            result.found = true;
            break;
        }
        
        for (int neighbor : mstGraph[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                parentMap[neighbor] = current;
                queue.push(neighbor);
            }
        }
    }
    
    if (result.found) {
        int current = destination;
        while (current != origin) {
            result.path.insert(result.path.begin(), current);
            current = parentMap[current];
        }
        result.path.insert(result.path.begin(), origin);
        
        int cost = 0;
        for (size_t i = 0; i < result.path.size() - 1; i++) {
            cost += graph->getEdgeWeight(result.path[i], result.path[i + 1]);
        }
        result.totalCost = cost;
        result.steps.push_back("MST path found with cost: " + std::to_string(cost));
    } else {
        result.steps.push_back("Destination not reachable in MST");
    }
    
    return result;
}

int KruskalAlgorithm::findParent(std::vector<int>& parent, int node) {
    if (parent[node] != node) {
        parent[node] = findParent(parent, parent[node]);
    }
    return parent[node];
}

void KruskalAlgorithm::unionSets(std::vector<int>& parent, std::vector<int>& rank, int a, int b) {
    int rootA = findParent(parent, a);
    int rootB = findParent(parent, b);
    
    if (rank[rootA] < rank[rootB]) {
        parent[rootA] = rootB;
    } else if (rank[rootA] > rank[rootB]) {
        parent[rootB] = rootA;
    } else {
        parent[rootB] = rootA;
        rank[rootA]++;
    }
}

std::string FloydWarshallAlgorithm::getName() const {
    return "Floyd-Warshall";
}

PathResult FloydWarshallAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "Floyd-Warshall";
    result.found = false;
    
    std::vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No stations in graph");
        return result;
    }
    
    int n = stations.size();
    std::map<int, int> stationIndex;
    std::map<int, int> indexStation;
    
    for (int i = 0; i < n; i++) {
        stationIndex[stations[i]] = i;
        indexStation[i] = stations[i];
    }
    
    const int INF = std::numeric_limits<int>::max() / 2;
    std::vector<std::vector<int>> dist(n, std::vector<int>(n, INF));
    std::vector<std::vector<int>> next(n, std::vector<int>(n, -1));
    
    for (int i = 0; i < n; i++) {
        dist[i][i] = 0;
    }
    
    result.steps.push_back("Initializing distance matrix");
    
    for (int i = 0; i < n; i++) {
        int stationId = indexStation[i];
        std::vector<int> neighbors = graph->getNeighbors(stationId);
        
        for (int neighbor : neighbors) {
            int j = stationIndex[neighbor];
            int weight = graph->getEdgeWeight(stationId, neighbor);
            dist[i][j] = weight;
            next[i][j] = j;
        }
    }
    
    result.steps.push_back("Starting Floyd-Warshall algorithm");
    
    VisualizationStep initStep;
    initStep.description = "Initializing all-pairs shortest paths";
    result.visualSteps.push_back(initStep);
    
    for (int k = 0; k < n; k++) {
        std::string stepDesc = "Processing intermediate vertex " + std::to_string(indexStation[k]);
        result.steps.push_back(stepDesc);
        
        VisualizationStep step;
        step.visitedNodes.push_back(indexStation[k]);
        step.description = stepDesc;
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    if (dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];
                        
                        step.visitedEdges.push_back({indexStation[i], indexStation[j]});
                    }
                }
            }
        }
        
        result.visualSteps.push_back(step);
    }
    
    result.steps.push_back("Floyd-Warshall complete, computing path");
    
    int startIdx = stationIndex[origin];
    int endIdx = stationIndex[destination];
    
    if (dist[startIdx][endIdx] == INF) {
        result.steps.push_back("No path exists between " + std::to_string(origin) + " and " + std::to_string(destination));
        result.found = false;
        return result;
    }
    
    result.path.push_back(origin);
    int current = startIdx;
    
    while (current != endIdx) {
        current = next[current][endIdx];
        if (current == -1) {
            result.found = false;
            result.steps.push_back("Path reconstruction failed");
            return result;
        }
        result.path.push_back(indexStation[current]);
    }
    
    result.totalCost = dist[startIdx][endIdx];
    result.found = true;
    
    std::string pathStr = "Path found: ";
    for (size_t i = 0; i < result.path.size(); i++) {
        pathStr += std::to_string(result.path[i]);
        if (i < result.path.size() - 1) pathStr += " -> ";
    }
    pathStr += " (Total: " + std::to_string(result.totalCost) + ")";
    result.steps.push_back(pathStr);
    
    VisualizationStep finalStep;
    finalStep.visitedNodes = result.path;
    for (size_t i = 0; i < result.path.size() - 1; i++) {
        finalStep.visitedEdges.push_back({result.path[i], result.path[i + 1]});
    }
    finalStep.description = "Final path from " + std::to_string(origin) + " to " + std::to_string(destination);
    result.visualSteps.push_back(finalStep);
    
    return result;
}

std::string KruskalAlgorithm::getName() const {
    return "Kruskal (MST)";
}
