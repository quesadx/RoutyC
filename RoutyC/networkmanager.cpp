#include "networkmanager.h"
#include "graphicsitems.h"
#include "mainwindow.h"
#include <QGraphicsScene>
#include <QBrush>
#include <QPen>
#include <QColor>

NetworkManager::NetworkManager(QGraphicsScene* scene, MainWindow* window)
    : scene(scene), mainWindow(window) {
    tree = new StationTree();
    graph = new TransportGraph();
}

NetworkManager::~NetworkManager() {
    delete tree;
    delete graph;
}

void NetworkManager::createStation(int id, const QString& name, double x, double y) {
    DraggableStation* station = new DraggableStation(id, x, y, 40, mainWindow);
    scene->addItem(station);
    
    QGraphicsTextItem* labelItem = new QGraphicsTextItem(name);
    labelItem->setDefaultTextColor(Qt::black);
    scene->addItem(labelItem);
    
    QRectF bounds = station->rect();
    QPointF center = bounds.center() + station->pos();
    labelItem->setPos(center.x() - labelItem->boundingRect().width() / 2,
                      center.y() + bounds.height() / 2);
    
    station->setLabel(labelItem);
    
    stationItems[id] = station;
    tree->insertStation(id, name.toStdString(), x, y);
    graph->addStation(id);
}

void NetworkManager::deleteStation(int stationId) {
    if (stationItems.find(stationId) == stationItems.end()) {
        return;
    }
    
    std::vector<int> neighbors = graph->getNeighbors(stationId);
    for (int neighborId : neighbors) {
        deleteRoute(stationId, neighborId);
    }
    
    DraggableStation* station = stationItems[stationId];
    QGraphicsTextItem* label = station->getLabel();
    
    if (label) {
        scene->removeItem(label);
        delete label;
    }
    
    scene->removeItem(station);
    delete station;
    
    stationItems.erase(stationId);
    tree->removeStation(stationId);
    graph->removeStation(stationId);
}

void NetworkManager::createRoute(int id1, int id2, int travelTime) {
    if (stationItems.find(id1) == stationItems.end() || 
        stationItems.find(id2) == stationItems.end()) {
        return;
    }
    
    DraggableStation* station1 = stationItems[id1];
    DraggableStation* station2 = stationItems[id2];
    
    QRectF bounds1 = station1->rect();
    QRectF bounds2 = station2->rect();
    
    QPointF center1 = bounds1.center() + station1->pos();
    QPointF center2 = bounds2.center() + station2->pos();
    
    ClickableRoute* route = new ClickableRoute(id1, id2, QLineF(center1, center2), mainWindow);
    scene->addItem(route);
    route->setZValue(-1);
    
    std::pair<int, int> key = normalizeEdgePair(id1, id2);
    routeItems[key] = route;
    
    graph->addEdge(id1, id2, travelTime);
}

void NetworkManager::deleteRoute(int id1, int id2) {
    std::pair<int, int> key = normalizeEdgePair(id1, id2);
    
    if (routeItems.find(key) == routeItems.end()) {
        return;
    }
    
    ClickableRoute* route = routeItems[key];
    scene->removeItem(route);
    delete route;
    
    routeItems.erase(key);
    graph->removeEdge(id1, id2);
}

void NetworkManager::updateRoutePosition(int stationId, const QPointF& center) {
    if (stationItems.find(stationId) == stationItems.end()) {
        return;
    }
    
    std::vector<int> neighbors = graph->getNeighbors(stationId);
    
    for (int neighborId : neighbors) {
        if (stationItems.find(neighborId) == stationItems.end()) {
            continue;
        }
        
        DraggableStation* neighbor = stationItems[neighborId];
        QRectF neighborBounds = neighbor->rect();
        QPointF neighborCenter = neighborBounds.center() + neighbor->pos();
        
        std::pair<int, int> key = normalizeEdgePair(stationId, neighborId);
        if (routeItems.find(key) != routeItems.end()) {
            ClickableRoute* route = routeItems[key];
            route->setLine(QLineF(center, neighborCenter));
        }
    }
    
    tree->updatePosition(stationId, center.x(), center.y());
}

void NetworkManager::clearAll() {
    scene->clear();
    stationItems.clear();
    routeItems.clear();
    tree->clear();
    graph->clear();
}

void NetworkManager::reconstructFromData(StationTree* sourceTree, TransportGraph* sourceGraph) {
    std::vector<StationNode*> stations = sourceTree->getAllStations();
    
    for (StationNode* node : stations) {
        DraggableStation* station = new DraggableStation(node->id, node->x, node->y, 40, mainWindow);
        scene->addItem(station);
        
        QGraphicsTextItem* labelItem = new QGraphicsTextItem(QString::fromStdString(node->name));
        labelItem->setDefaultTextColor(Qt::black);
        scene->addItem(labelItem);
        
        QRectF bounds = station->rect();
        QPointF center = bounds.center() + station->pos();
        labelItem->setPos(center.x() - labelItem->boundingRect().width() / 2,
                          center.y() + bounds.height() / 2);
        
        station->setLabel(labelItem);
        stationItems[node->id] = station;
    }
    
    std::vector<int> allStations = sourceGraph->getAllStations();
    for (int stationId : allStations) {
        std::vector<int> neighbors = sourceGraph->getNeighbors(stationId);
        for (int neighbor : neighbors) {
            if (stationId < neighbor) {
                std::pair<int, int> key = normalizeEdgePair(stationId, neighbor);
                if (routeItems.find(key) == routeItems.end()) {
                    DraggableStation* station1 = stationItems[stationId];
                    DraggableStation* station2 = stationItems[neighbor];
                    
                    QRectF bounds1 = station1->rect();
                    QRectF bounds2 = station2->rect();
                    
                    QPointF center1 = bounds1.center() + station1->pos();
                    QPointF center2 = bounds2.center() + station2->pos();
                    
                    ClickableRoute* route = new ClickableRoute(stationId, neighbor, 
                                                               QLineF(center1, center2), mainWindow);
                    scene->addItem(route);
                    route->setZValue(-1);
                    
                    routeItems[key] = route;
                }
            }
        }
    }
}

StationTree* NetworkManager::getTree() {
    return tree;
}

TransportGraph* NetworkManager::getGraph() {
    return graph;
}

DraggableStation* NetworkManager::getStation(int id) {
    if (stationItems.find(id) != stationItems.end()) {
        return stationItems[id];
    }
    return nullptr;
}

std::vector<StationNode*> NetworkManager::getAllStations() {
    return tree->getAllStations();
}

void NetworkManager::highlightPath(const std::vector<int>& path) {
    clearHighlights();
    
    for (int stationId : path) {
        if (stationItems.find(stationId) != stationItems.end()) {
            stationItems[stationId]->setBrush(QBrush(QColor(168, 85, 247)));
        }
    }
    
    for (size_t i = 0; i < path.size() - 1; i++) {
        std::pair<int, int> key = normalizeEdgePair(path[i], path[i + 1]);
        if (routeItems.find(key) != routeItems.end()) {
            routeItems[key]->setPen(QPen(QColor(168, 85, 247), 4));
        }
    }
}

void NetworkManager::clearHighlights() {
    for (auto& pair : stationItems) {
        pair.second->setBrush(QBrush(QColor(124, 58, 237)));
    }
    
    for (auto& pair : routeItems) {
        pair.second->setPen(QPen(QColor(107, 114, 128), 2));
    }
}

std::pair<int, int> NetworkManager::normalizeEdgePair(int a, int b) {
    if (a < b) {
        return std::make_pair(a, b);
    }
    return std::make_pair(b, a);
}
