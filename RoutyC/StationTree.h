#ifndef STATIONTREE_H
#define STATIONTREE_H

#include <string>
#include <vector>

using namespace std;

struct StationNode {
    int id;
    string name;
    double x;
    double y;
    StationNode* left;
    StationNode* right;
    
    StationNode(int stationId, const string& stationName, double posX, double posY);
};

class StationTree {
public:
    StationTree();
    ~StationTree();
    
    void addStation(int id, const string& name, double x, double y);
    bool removeStation(int id);
    StationNode* searchStation(int id);
    void updatePosition(int id, double x, double y);
    vector<StationNode*> getAllStations();
    vector<StationNode*> getPreOrderTraversal();
    vector<StationNode*> getPostOrderTraversal();
    void clear();
    
private:
    StationNode* root;
    
    StationNode* addNodeToTree(StationNode* node, int id, const string& name, double x, double y);
    StationNode* removeNodeFromTree(StationNode* node, int id);
    StationNode* findSmallestNode(StationNode* node);
    StationNode* searchInTree(StationNode* node, int id);
    void collectInOrder(StationNode* node, vector<StationNode*>& nodes);
    void collectPreOrder(StationNode* node, vector<StationNode*>& nodes);
    void collectPostOrder(StationNode* node, vector<StationNode*>& nodes);
    void deleteAllNodes(StationNode* node);
};

#endif
