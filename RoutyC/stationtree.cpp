#include "stationtree.h"

StationNode::StationNode(int stationId, const std::string& stationName, double posX, double posY)
    : id(stationId), name(stationName), x(posX), y(posY), left(nullptr), right(nullptr) {
}

StationTree::StationTree() : root(nullptr) {
}

StationTree::~StationTree() {
    destroyTree(root);
}

void StationTree::insertStation(int id, const std::string& name, double x, double y) {
    root = insertNode(root, id, name, x, y);
}

StationNode* StationTree::insertNode(StationNode* node, int id, const std::string& name, double x, double y) {
    if (node == nullptr) {
        return new StationNode(id, name, x, y);
    }
    
    if (id < node->id) {
        node->left = insertNode(node->left, id, name, x, y);
    } else if (id > node->id) {
        node->right = insertNode(node->right, id, name, x, y);
    }
    
    return node;
}

bool StationTree::removeStation(int id) {
    int initialSize = getAllStations().size();
    root = removeNode(root, id);
    int finalSize = getAllStations().size();
    return finalSize < initialSize;
}

StationNode* StationTree::removeNode(StationNode* node, int id) {
    if (node == nullptr) {
        return nullptr;
    }
    
    if (id < node->id) {
        node->left = removeNode(node->left, id);
    } else if (id > node->id) {
        node->right = removeNode(node->right, id);
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
        
        StationNode* temp = findMinNode(node->right);
        node->id = temp->id;
        node->name = temp->name;
        node->x = temp->x;
        node->y = temp->y;
        node->right = removeNode(node->right, temp->id);
    }
    
    return node;
}

StationNode* StationTree::findMinNode(StationNode* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

StationNode* StationTree::findStation(int id) {
    return searchNode(root, id);
}

StationNode* StationTree::searchNode(StationNode* node, int id) {
    if (node == nullptr || node->id == id) {
        return node;
    }
    
    if (id < node->id) {
        return searchNode(node->left, id);
    }
    return searchNode(node->right, id);
}

void StationTree::updatePosition(int id, double x, double y) {
    StationNode* node = findStation(id);
    if (node != nullptr) {
        node->x = x;
        node->y = y;
    }
}

std::vector<StationNode*> StationTree::getAllStations() {
    std::vector<StationNode*> stations;
    collectNodes(root, stations);
    return stations;
}

void StationTree::collectNodes(StationNode* node, std::vector<StationNode*>& nodes) {
    if (node != nullptr) {
        collectNodes(node->left, nodes);
        nodes.push_back(node);
        collectNodes(node->right, nodes);
    }
}

void StationTree::clear() {
    destroyTree(root);
    root = nullptr;
}

void StationTree::destroyTree(StationNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}
