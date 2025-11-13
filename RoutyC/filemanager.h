#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include "stationtree.h"
#include "transportgraph.h"

class FileManager {
public:
    static bool saveToFile(const std::string& filename, StationTree* tree, TransportGraph* graph);
    static bool loadFromFile(const std::string& filename, StationTree* tree, TransportGraph* graph);
    static bool loadClosures(const std::string& filename, TransportGraph* graph);
    static bool saveClosures(const std::string& filename, TransportGraph* graph);
    
private:
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
};

#endif
