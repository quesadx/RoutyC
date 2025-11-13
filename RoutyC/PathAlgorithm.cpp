#include "pathalgorithm.h"
#include <queue>
#include <map>
#include <set>
#include <limits>
#include <algorithm>

using namespace std;

PathResult::PathResult() : totalCost(0), algorithmName(""), found(false) {
}

PathAlgorithm::~PathAlgorithm() {
}

PathResult DijkstraAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "Dijkstra";
    result.found = false;
    
    vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No hay estaciones en el grafo");
        return result;
    }
    
    map<int, int> distances;
    map<int, int> previous;
    set<int> unvisited;
    set<int> visited;
    
    for (int station : stations) {
        distances[station] = numeric_limits<int>::max();
        unvisited.insert(station);
    }
    
    distances[origin] = 0;
    result.steps.push_back("Iniciando Dijkstra desde estación " + to_string(origin));
    
    VisualizationStep initStep;
    initStep.visitedNodes.push_back(origin);
    initStep.description = "Iniciando desde estación " + to_string(origin);
    result.visualSteps.push_back(initStep);
    
    while (!unvisited.empty()) {
        int current = -1;
        int minDist = numeric_limits<int>::max();
        
        for (int station : unvisited) {
            if (distances[station] < minDist) {
                minDist = distances[station];
                current = station;
            }
        }
        
        if (current == -1 || distances[current] == numeric_limits<int>::max()) {
            break;
        }
        
        unvisited.erase(current);
        visited.insert(current);
        result.steps.push_back("Visitando estación " + to_string(current) + 
                              " (distancia: " + to_string(distances[current]) + ")");
        
        VisualizationStep step;
        for (int v : visited) {
            step.visitedNodes.push_back(v);
        }
        for (int v : visited) {
            if (previous.find(v) != previous.end()) {
                step.visitedEdges.push_back({previous[v], v});
            }
        }
        step.description = "Visitando estación " + to_string(current) + " (distancia: " + to_string(distances[current]) + ")";
        result.visualSteps.push_back(step);
        
        if (current == destination) {
            result.found = true;
            break;
        }
        
        vector<int> neighbors = graph->getConnectedStations(current);
        for (int neighbor : neighbors) {
            if (unvisited.find(neighbor) == unvisited.end()) {
                continue;
            }
            
            int weight = graph->getRouteTime(current, neighbor);
            int newDist = distances[current] + weight;
            
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                previous[neighbor] = current;
                result.steps.push_back("  Actualizada estación " + to_string(neighbor) + 
                                      " distancia a " + to_string(newDist));
            }
        }
    }
    
    if (previous.find(destination) != previous.end()) {
        result.found = true;
        
        int current = destination;
        while (current != origin) {
            result.path.insert(result.path.begin(), current);
            current = previous[current];
        }
        result.path.insert(result.path.begin(), origin);
        
        result.totalCost = static_cast<int>(result.path.size()) - 1;
        result.steps.push_back("Ruta encontrada con costo total: " + to_string(result.totalCost));
    } else {
    }
    
    if (result.found) {
        int current = destination;
        while (current != origin) {
            result.path.insert(result.path.begin(), current);
            if (previous.find(current) == previous.end()) {
                result.found = false;
                result.steps.push_back("Falló la reconstrucción de la ruta");
                return result;
            }
            current = previous[current];
        }
        result.path.insert(result.path.begin(), origin);
        result.totalCost = distances[destination];
        result.steps.push_back("Ruta encontrada con costo total: " + to_string(result.totalCost));
    } else {
        result.steps.push_back("No se encontró ruta al destino");
    }
    
    return result;
}

string DijkstraAlgorithm::getName() const {
    return "Dijkstra";
}

PathResult BFSAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "BFS";
    result.found = false;
    
    vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No hay estaciones en el grafo");
        return result;
    }
    
    map<int, bool> visited;
    map<int, int> previous;
    queue<int> queue;
    vector<int> visitedOrder;
    
    for (int station : stations) {
        visited[station] = false;
    }
    
    queue.push(origin);
    visited[origin] = true;
    visitedOrder.push_back(origin);
    result.steps.push_back("Iniciando BFS desde estación " + to_string(origin));
    
    VisualizationStep initStep;
    initStep.visitedNodes.push_back(origin);
    initStep.description = "Iniciando BFS desde estación " + to_string(origin);
    result.visualSteps.push_back(initStep);
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        result.steps.push_back("Visitando estación " + to_string(current));
        
        VisualizationStep step;
        for (int v : visitedOrder) {
            step.visitedNodes.push_back(v);
        }
        for (int v : visitedOrder) {
            if (previous.find(v) != previous.end()) {
                step.visitedEdges.push_back({previous[v], v});
            }
        }
        step.description = "Visitando estación " + to_string(current);
        result.visualSteps.push_back(step);
        
        if (current == destination) {
            result.found = true;
            break;
        }
        
        vector<int> neighbors = graph->getConnectedStations(current);
        for (int neighbor : neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                previous[neighbor] = current;
                queue.push(neighbor);
                visitedOrder.push_back(neighbor);
                result.steps.push_back("  Encolada estación " + to_string(neighbor));
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
                result.steps.push_back("Falló la reconstrucción de la ruta");
                return result;
            }
            int prev = previous[current];
            cost += graph->getRouteTime(prev, current);
            current = prev;
        }
        result.path.insert(result.path.begin(), origin);
        result.totalCost = cost;
        result.steps.push_back("Ruta encontrada con costo total: " + to_string(result.totalCost));
    } else {
        result.steps.push_back("No se encontró ruta al destino");
    }
    
    return result;
}

string BFSAlgorithm::getName() const {
    return "BFS (Breadth-First Search)";
}

PathResult DFSAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "DFS";
    result.found = false;
    
    vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No hay estaciones en el grafo");
        return result;
    }
    
    map<int, bool> visitedMap;
    for (int station : stations) {
        visitedMap[station] = false;
    }
    
    vector<bool> visited(10000, false);
    vector<int> path;
    vector<int> foundPath;
    
    result.steps.push_back("Iniciando DFS desde la estación " + to_string(origin));
    
    VisualizationStep initStep;
    initStep.visitedNodes.push_back(origin);
    initStep.description = "Iniciando DFS desde la estación " + to_string(origin);
    result.visualSteps.push_back(initStep);
    
    for (int station : stations) {
        visited[station] = visitedMap[station];
    }
    
    result.found = dfsRecursive(graph, origin, destination, visited, path, foundPath, result.steps, result.visualSteps);
    
    if (result.found) {
        result.path = foundPath;
        int cost = 0;
        for (size_t i = 0; i < result.path.size() - 1; i++) {
            cost += graph->getRouteTime(result.path[i], result.path[i + 1]);
        }
        result.totalCost = cost;
        result.steps.push_back("Ruta encontrada con costo total: " + to_string(result.totalCost));
    } else {
        result.steps.push_back("No se encontró ruta al destino");
    }
    
    return result;
}

bool DFSAlgorithm::dfsRecursive(TransportGraph* graph, int current, int destination,
                                vector<bool>& visited, vector<int>& path,
                                vector<int>& result, vector<string>& steps,
                                vector<VisualizationStep>& visualSteps) {
    visited[current] = true;
    path.push_back(current);
    steps.push_back("Visitando estación " + to_string(current));
    
    VisualizationStep step;
    for (int node : path) {
        step.visitedNodes.push_back(node);
    }
    for (size_t i = 1; i < path.size(); i++) {
        step.visitedEdges.push_back({path[i-1], path[i]});
    }
    step.description = "Visitando estación " + to_string(current);
    visualSteps.push_back(step);
    
    if (current == destination) {
        result = path;
        return true;
    }
    
    vector<int> neighbors = graph->getConnectedStations(current);
    for (int neighbor : neighbors) {
        if (!visited[neighbor]) {
            if (dfsRecursive(graph, neighbor, destination, visited, path, result, steps, visualSteps)) {
                return true;
            }
        }
    }
    
    path.pop_back();
    steps.push_back("Retrocediendo desde estación " + to_string(current));
    
    VisualizationStep backtrackStep;
    for (int node : path) {
        backtrackStep.visitedNodes.push_back(node);
    }
    for (size_t i = 1; i < path.size(); i++) {
        backtrackStep.visitedEdges.push_back({path[i-1], path[i]});
    }
    backtrackStep.description = "Retrocediendo desde estación " + to_string(current);
    visualSteps.push_back(backtrackStep);
    
    return false;
}

string DFSAlgorithm::getName() const {
    return "DFS (Depth-First Search)";
}

PathResult PrimAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "Prim MST";
    result.found = false;
    
    vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No stations in graph");
        return result;
    }
    
    map<int, bool> inMST;
    map<int, int> key;
    map<int, int> parent;
    
    for (int station : stations) {
        inMST[station] = false;
        key[station] = numeric_limits<int>::max();
    }
    
    key[origin] = 0;
    parent[origin] = -1;
    result.steps.push_back("Iniciando MST de Prim desde la estación " + to_string(origin));
    
    VisualizationStep initStep;
    initStep.visitedNodes.push_back(origin);
    initStep.description = "Iniciando desde la estación " + to_string(origin);
    result.visualSteps.push_back(initStep);
    
    for (size_t count = 0; count < stations.size(); count++) {
        int u = -1;
        int minKey = numeric_limits<int>::max();
        
        for (int station : stations) {
            if (!inMST[station] && key[station] < minKey) {
                minKey = key[station];
                u = station;
            }
        }
        
        if (u == -1) break;
        
        inMST[u] = true;
        result.steps.push_back("Estación " + to_string(u) + " añadida al MST");
        
        VisualizationStep step;
        for (int station : stations) {
            if (inMST[station]) {
                step.visitedNodes.push_back(station);
            }
        }
        if (parent[u] != -1) {
            step.visitedEdges.push_back({parent[u], u});
        }
        step.description = "Estación " + to_string(u) + " añadida al MST";
        result.visualSteps.push_back(step);
        
        vector<int> neighbors = graph->getConnectedStations(u);
        for (int v : neighbors) {
            int weight = graph->getRouteTime(u, v);
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
            cost += graph->getRouteTime(result.path[i], result.path[i + 1]);
        }
        result.totalCost = cost;
        result.steps.push_back("Camino MST encontrado con costo: " + to_string(cost));
    } else {
        result.steps.push_back("Destino no alcanzable en MST");
    }
    
    return result;
}

string PrimAlgorithm::getName() const {
    return "Prim (MST)";
}

PathResult KruskalAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "Kruskal (MST)";
    result.found = false;
    
    vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No hay estaciones en el grafo");
        return result;
    }
    
    vector<pair<int, pair<int, int>>> edges;
    for (int station : stations) {
        vector<int> neighbors = graph->getConnectedStations(station);
        for (int neighbor : neighbors) {
            if (station < neighbor) {
                int weight = graph->getRouteTime(station, neighbor);
                edges.push_back({weight, {station, neighbor}});
            }
        }
    }
    
    sort(edges.begin(), edges.end());
    
    vector<int> parent(10000);
    vector<int> rank(10000, 0);
    for (int station : stations) {
        parent[station] = station;
    }
    
    result.steps.push_back("Iniciando MST Kruskal");
    vector<pair<int, int>> mstEdges;
    
    for (auto& edge : edges) {
        int weight = edge.first;
        int u = edge.second.first;
        int v = edge.second.second;
        
        int setU = findParent(parent, u);
        int setV = findParent(parent, v);
        
        if (setU != setV) {
            mstEdges.push_back({u, v});
            unionSets(parent, rank, setU, setV);
            result.steps.push_back("Ruta añadida " + to_string(u) + "-" + 
                                  to_string(v) + " (peso: " + to_string(weight) + ")");
            
            VisualizationStep step;
            set<int> connectedNodes;
            for (auto& e : mstEdges) {
                connectedNodes.insert(e.first);
                connectedNodes.insert(e.second);
                step.visitedEdges.push_back(e);
            }
            for (int node : connectedNodes) {
                step.visitedNodes.push_back(node);
            }
            step.description = "Ruta añadida " + to_string(u) + "-" + to_string(v);
            result.visualSteps.push_back(step);
        }
    }
    
    map<int, vector<int>> mstGraph;
    for (auto& edge : mstEdges) {
        mstGraph[edge.first].push_back(edge.second);
        mstGraph[edge.second].push_back(edge.first);
    }
    
    map<int, bool> visited;
    map<int, int> parentMap;
    queue<int> queue;
    
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
            cost += graph->getRouteTime(result.path[i], result.path[i + 1]);
        }
        result.totalCost = cost;
        result.steps.push_back("Camino MST encontrado con costo: " + to_string(cost));
    } else {
        result.steps.push_back("Destino no alcanzable en MST");
    }
    
    return result;
}

int KruskalAlgorithm::findParent(vector<int>& parent, int node) {
    if (parent[node] != node) {
        parent[node] = findParent(parent, parent[node]);
    }
    return parent[node];
}

void KruskalAlgorithm::unionSets(vector<int>& parent, vector<int>& rank, int a, int b) {
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

string FloydWarshallAlgorithm::getName() const {
    return "Floyd-Warshall";
}

PathResult FloydWarshallAlgorithm::findPath(TransportGraph* graph, int origin, int destination) {
    PathResult result;
    result.algorithmName = "Floyd-Warshall";
    result.found = false;
    
    vector<int> stations = graph->getAllStations();
    if (stations.empty()) {
        result.steps.push_back("No hay estaciones en el grafo");
        return result;
    }
    
    int n = stations.size();
    map<int, int> stationIndex;
    map<int, int> indexStation;
    
    for (int i = 0; i < n; i++) {
        stationIndex[stations[i]] = i;
        indexStation[i] = stations[i];
    }
    
    const int INF = numeric_limits<int>::max() / 2;
    vector<vector<int>> dist(n, vector<int>(n, INF));
    vector<vector<int>> next(n, vector<int>(n, -1));
    
    for (int i = 0; i < n; i++) {
        dist[i][i] = 0;
    }
    
    result.steps.push_back("Inicializando matriz de distancias");
    
    for (int i = 0; i < n; i++) {
        int stationId = indexStation[i];
        vector<int> neighbors = graph->getConnectedStations(stationId);
        
        for (int neighbor : neighbors) {
            int j = stationIndex[neighbor];
            int weight = graph->getRouteTime(stationId, neighbor);
            dist[i][j] = weight;
            next[i][j] = j;
        }
    }
    
    result.steps.push_back("Iniciando algoritmo Floyd-Warshall");
    
    VisualizationStep initStep;
    initStep.description = "Inicializando caminos más cortos entre todos los pares";
    result.visualSteps.push_back(initStep);
    
    for (int k = 0; k < n; k++) {
        string stepDesc = "Procesando vértice intermedio " + to_string(indexStation[k]);
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
    
    result.steps.push_back("Floyd-Warshall completado, calculando ruta");
    
    int startIdx = stationIndex[origin];
    int endIdx = stationIndex[destination];
    
    if (dist[startIdx][endIdx] == INF) {
        result.steps.push_back("No existe ruta entre " + to_string(origin) + " y " + to_string(destination));
        result.found = false;
        return result;
    }
    
    result.path.push_back(origin);
    int current = startIdx;
    
    while (current != endIdx) {
        current = next[current][endIdx];
        if (current == -1) {
            result.found = false;
            result.steps.push_back("Falló la reconstrucción de la ruta");
            return result;
        }
        result.path.push_back(indexStation[current]);
    }
    
    result.totalCost = dist[startIdx][endIdx];
    result.found = true;
    
    string pathStr = "Ruta encontrada: ";
    for (size_t i = 0; i < result.path.size(); i++) {
        pathStr += to_string(result.path[i]);
        if (i < result.path.size() - 1) pathStr += " > ";
    }
    pathStr += " (Total: " + to_string(result.totalCost) + ")";
    result.steps.push_back(pathStr);
    
    VisualizationStep finalStep;
    finalStep.visitedNodes = result.path;
    for (size_t i = 0; i < result.path.size() - 1; i++) {
        finalStep.visitedEdges.push_back({result.path[i], result.path[i + 1]});
    }
    finalStep.description = "Ruta final desde " + to_string(origin) + " hasta " + to_string(destination);
    result.visualSteps.push_back(finalStep);
    
    return result;
}

string KruskalAlgorithm::getName() const {
    return "Kruskal (MST)";
}
