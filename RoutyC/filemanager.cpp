#include "filemanager.h"
#include <fstream>
#include <sstream>

bool FileManager::saveToFile(const std::string& filename, StationTree* tree, TransportGraph* graph) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::vector<StationNode*> stations = tree->getAllStations();
    
    file << "STATIONS\n";
    for (StationNode* node : stations) {
        file << node->id << "|" 
             << escapeString(node->name) << "|"
             << node->x << "|"
             << node->y << "\n";
    }
    
    file << "ROUTES\n";
    std::vector<int> allStations = graph->getAllStations();
    for (int stationId : allStations) {
        std::vector<int> neighbors = graph->getNeighbors(stationId);
        for (int neighbor : neighbors) {
            if (stationId < neighbor) {
                int weight = graph->getEdgeWeight(stationId, neighbor);
                file << stationId << "|" << neighbor << "|" << weight << "\n";
            }
        }
    }
    
    file.close();
    return true;
}

bool FileManager::loadFromFile(const std::string& filename, StationTree* tree, TransportGraph* graph) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    tree->clear();
    graph->clear();
    
    std::string line;
    std::string section;
    
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        
        if (line == "STATIONS") {
            section = "STATIONS";
            continue;
        } else if (line == "ROUTES") {
            section = "ROUTES";
            continue;
        }
        
        if (section == "STATIONS") {
            std::istringstream iss(line);
            std::string idStr, name, xStr, yStr;
            
            std::getline(iss, idStr, '|');
            std::getline(iss, name, '|');
            std::getline(iss, xStr, '|');
            std::getline(iss, yStr, '|');
            
            int id = std::stoi(idStr);
            double x = std::stod(xStr);
            double y = std::stod(yStr);
            std::string unescapedName = unescapeString(name);
            
            tree->insertStation(id, unescapedName, x, y);
            graph->addStation(id);
            
        } else if (section == "ROUTES") {
            std::istringstream iss(line);
            std::string id1Str, id2Str, weightStr;
            
            std::getline(iss, id1Str, '|');
            std::getline(iss, id2Str, '|');
            std::getline(iss, weightStr, '|');
            
            int id1 = std::stoi(id1Str);
            int id2 = std::stoi(id2Str);
            int weight = std::stoi(weightStr);
            
            graph->addEdge(id1, id2, weight);
        }
    }
    
    file.close();
    return true;
}

std::string FileManager::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '|') {
            result += "\\|";
        } else if (c == '\\') {
            result += "\\\\";
        } else if (c == '\n') {
            result += "\\n";
        } else {
            result += c;
        }
    }
    return result;
}

std::string FileManager::unescapeString(const std::string& str) {
    std::string result;
    bool escaped = false;
    
    for (char c : str) {
        if (escaped) {
            if (c == '|') {
                result += '|';
            } else if (c == '\\') {
                result += '\\';
            } else if (c == 'n') {
                result += '\n';
            } else {
                result += c;
            }
            escaped = false;
        } else {
            if (c == '\\') {
                escaped = true;
            } else {
                result += c;
            }
        }
    }
    
    return result;
}

bool FileManager::loadClosures(const std::string& filename, TransportGraph* graph) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    graph->clearClosures();
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        
        std::istringstream iss(line);
        std::string id1Str, id2Str;
        
        std::getline(iss, id1Str, '|');
        std::getline(iss, id2Str, '|');
        
        int id1 = std::stoi(id1Str);
        int id2 = std::stoi(id2Str);
        
        graph->addClosure(id1, id2);
    }
    
    file.close();
    return true;
}

bool FileManager::saveClosures(const std::string& filename, TransportGraph* graph) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::vector<std::pair<int, int>> closures = graph->getClosures();
    for (const auto& closure : closures) {
        file << closure.first << "|" << closure.second << "\n";
    }
    
    file.close();
    return true;
}
