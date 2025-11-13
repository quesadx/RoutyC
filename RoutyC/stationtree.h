#ifndef STATIONTREE_H
#define STATIONTREE_H

#include <string>
#include <vector>

struct StationNode {
    int id;
    std::string name;
    double x;
    double y;
    StationNode* left;
    StationNode* right;
    
    StationNode(int stationId, const std::string& stationName, double posX, double posY);
};

class StationTree {
public:
    StationTree();
    ~StationTree();
    
    void insertStation(int id, const std::string& name, double x, double y);
    bool removeStation(int id);
    StationNode* findStation(int id);
    void updatePosition(int id, double x, double y);
    std::vector<StationNode*> getAllStations();
    std::vector<StationNode*> getPreOrderTraversal();
    std::vector<StationNode*> getPostOrderTraversal();
    void clear();
    
private:
    StationNode* root;
    
    StationNode* insertNode(StationNode* node, int id, const std::string& name, double x, double y);
    StationNode* removeNode(StationNode* node, int id);
    StationNode* findMinNode(StationNode* node);
    StationNode* searchNode(StationNode* node, int id);
    void collectNodes(StationNode* node, std::vector<StationNode*>& nodes);
    void collectPreOrder(StationNode* node, std::vector<StationNode*>& nodes);
    void collectPostOrder(StationNode* node, std::vector<StationNode*>& nodes);
    void destroyTree(StationNode* node);
};

#endif
