#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include "StationTree.h"
#include "TransportGraph.h"

using namespace std;

class FileManager {
public:
    static bool saveToFile(const string& filename, StationTree* tree, TransportGraph* graph);
    static bool loadFromFile(const string& filename, StationTree* tree, TransportGraph* graph);
    static bool loadClosures(const string& filename, TransportGraph* graph);
    static bool saveClosures(const string& filename, TransportGraph* graph);
    
private:
    static string escapeString(const string& str);
    static string unescapeString(const string& str);
};

#endif
