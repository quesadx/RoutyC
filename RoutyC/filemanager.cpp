#include "filemanager.h"
#include <fstream>
#include <sstream>

using namespace std;

bool FileManager::saveToFile(const string& filename, StationTree* tree, TransportGraph* graph) {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    vector<StationNode*> stations = tree->getAllStations();
    
    file << "STATIONS\n";
    for (StationNode* node : stations) {
        file << node->id << "|" 
             << escapeString(node->name) << "|"
             << node->x << "|"
             << node->y << "\n";
    }
    
    file << "ROUTES\n";
    vector<int> allStations = graph->getAllStations();
    for (int stationId : allStations) {
        vector<int> neighbors = graph->getConnectedStations(stationId);
        for (int neighbor : neighbors) {
            if (stationId < neighbor) {
                int weight = graph->getRouteTime(stationId, neighbor);
                file << stationId << "|" << neighbor << "|" << weight << "\n";
            }
        }
    }
    
    file.close();
    return true;
}

bool FileManager::loadFromFile(const string& filename, StationTree* tree, TransportGraph* graph) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    tree->clear();
    graph->clear();
    
    string line;
    string section;
    
    while (getline(file, line)) {
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
            istringstream iss(line);
            string idStr, name, xStr, yStr;
            
            getline(iss, idStr, '|');
            getline(iss, name, '|');
            getline(iss, xStr, '|');
            getline(iss, yStr, '|');
            
            int id = stoi(idStr);
            double x = stod(xStr);
            double y = stod(yStr);
            string unescapedName = unescapeString(name);
            
            tree->addStation(id, unescapedName, x, y);
            graph->addStation(id);
            
        } else if (section == "ROUTES") {
            istringstream iss(line);
            string id1Str, id2Str, weightStr;
            
            getline(iss, id1Str, '|');
            getline(iss, id2Str, '|');
            getline(iss, weightStr, '|');
            
            int id1 = stoi(id1Str);
            int id2 = stoi(id2Str);
            int weight = stoi(weightStr);
            
            graph->addRoute(id1, id2, weight);
        }
    }
    
    file.close();
    return true;
}

string FileManager::escapeString(const string& str) {
    string result;
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

string FileManager::unescapeString(const string& str) {
    string result;
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

bool FileManager::loadClosures(const string& filename, TransportGraph* graph) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    graph->clearBlockedRoutes();
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        
        istringstream iss(line);
        string id1Str, id2Str;
        
        getline(iss, id1Str, '|');
        getline(iss, id2Str, '|');
        
        int id1 = stoi(id1Str);
        int id2 = stoi(id2Str);
        
        graph->blockRoute(id1, id2);
    }
    
    file.close();
    return true;
}

bool FileManager::saveClosures(const string& filename, TransportGraph* graph) {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    vector<pair<int, int>> closures = graph->getBlockedRoutes();
    for (const auto& closure : closures) {
        file << closure.first << "|" << closure.second << "\n";
    }
    
    file.close();
    return true;
}
