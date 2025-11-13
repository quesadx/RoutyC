#include "stationtree.h"

using namespace std;

StationNode::StationNode(int stationId, const string& stationName, double posX, double posY)
    : id(stationId), name(stationName), x(posX), y(posY), left(nullptr), right(nullptr) {
}

StationTree::StationTree() : root(nullptr) {
}

StationTree::~StationTree() {
    deleteAllNodes(root);
}

void StationTree::addStation(int id, const string& name, double x, double y) {
    root = addNodeToTree(root, id, name, x, y);
}

StationNode* StationTree::addNodeToTree(StationNode* node, int id, const string& name, double x, double y) {
    if (node == nullptr) {
        return new StationNode(id, name, x, y);
    }
    
    if (id < node->id) {
        node->left = addNodeToTree(node->left, id, name, x, y);
    } else if (id > node->id) {
        node->right = addNodeToTree(node->right, id, name, x, y);
    }
    
    return node;
}

bool StationTree::removeStation(int id) {
    int initialSize = getAllStations().size();
    root = removeNodeFromTree(root, id);
    int finalSize = getAllStations().size();
    return finalSize < initialSize;
}

StationNode* StationTree::removeNodeFromTree(StationNode* node, int id) {
    if (node == nullptr) {
        return nullptr;
    }
    
    if (id < node->id) {
        node->left = removeNodeFromTree(node->left, id);
    } else if (id > node->id) {
        node->right = removeNodeFromTree(node->right, id);
    } else {
        if (node->left == nullptr) {
            StationNode* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            StationNode* temp = node->left;
            delete node;
            return temp;
        }
        
        StationNode* temp = findSmallestNode(node->right);
        node->id = temp->id;
        node->name = temp->name;
        node->x = temp->x;
        node->y = temp->y;
        node->right = removeNodeFromTree(node->right, temp->id);
    }
    
    return node;
}

StationNode* StationTree::findSmallestNode(StationNode* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

StationNode* StationTree::searchStation(int id) {
    return searchInTree(root, id);
}

StationNode* StationTree::searchInTree(StationNode* node, int id) {
    if (node == nullptr || node->id == id) {
        return node;
    }
    
    if (id < node->id) {
        return searchInTree(node->left, id);
    }
    return searchInTree(node->right, id);
}

void StationTree::updatePosition(int id, double x, double y) {
    StationNode* node = searchStation(id);
    if (node != nullptr) {
        node->x = x;
        node->y = y;
    }
}

vector<StationNode*> StationTree::getAllStations() {
    vector<StationNode*> stations;
    collectInOrder(root, stations);
    return stations;
}

void StationTree::collectInOrder(StationNode* node, vector<StationNode*>& nodes) {
    if (node != nullptr) {
        collectInOrder(node->left, nodes);
        nodes.push_back(node);
        collectInOrder(node->right, nodes);
    }
}

vector<StationNode*> StationTree::getPreOrderTraversal() {
    vector<StationNode*> stations;
    collectPreOrder(root, stations);
    return stations;
}

void StationTree::collectPreOrder(StationNode* node, vector<StationNode*>& nodes) {
    if (node != nullptr) {
        nodes.push_back(node);
        collectPreOrder(node->left, nodes);
        collectPreOrder(node->right, nodes);
    }
}

vector<StationNode*> StationTree::getPostOrderTraversal() {
    vector<StationNode*> stations;
    collectPostOrder(root, stations);
    return stations;
}

void StationTree::collectPostOrder(StationNode* node, vector<StationNode*>& nodes) {
    if (node != nullptr) {
        collectPostOrder(node->left, nodes);
        collectPostOrder(node->right, nodes);
        nodes.push_back(node);
    }
}

void StationTree::clear() {
    deleteAllNodes(root);
    root = nullptr;
}

void StationTree::deleteAllNodes(StationNode* node) {
    if (node != nullptr) {
        deleteAllNodes(node->left);
        deleteAllNodes(node->right);
        delete node;
    }
}
