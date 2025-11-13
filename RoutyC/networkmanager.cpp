#include "networkmanager.h"
#include "graphicsitems.h"
#include "mainwindow.h"
#include <QGraphicsScene>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QFont>

using namespace std;

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
    DraggableStation* station = new DraggableStation(id, x, y, 32, mainWindow);
    scene->addItem(station);
    
    QGraphicsTextItem* labelItem = new QGraphicsTextItem(name);
    labelItem->setDefaultTextColor(QColor(255, 255, 255));
    scene->addItem(labelItem);
    
    QRectF bounds = station->rect();
    QPointF center = bounds.center() + station->pos();
    labelItem->setPos(center.x() - labelItem->boundingRect().width() / 2,
                      center.y() + bounds.height() / 2);
    
    station->setLabel(labelItem);
    
    stations[id] = station;
    tree->addStation(id, name.toStdString(), x, y);
    graph->addStation(id);
}

void NetworkManager::deleteStation(int stationId) {
    if (stations.find(stationId) == stations.end()) {
        return;
    }
    
    std::vector<int> neighbors = graph->getConnectedStations(stationId);
    for (int neighborId : neighbors) {
        deleteRoute(stationId, neighborId);
    }
    
    DraggableStation* station = stations[stationId];
    QGraphicsTextItem* label = station->getLabel();
    
    if (label) {
        scene->removeItem(label);
        delete label;
    }
    
    scene->removeItem(station);
    delete station;
    
    stations.erase(stationId);
    tree->removeStation(stationId);
    graph->removeStation(stationId);
}

void NetworkManager::createRoute(int id1, int id2, int travelTime) {
    if (stations.find(id1) == stations.end() || 
        stations.find(id2) == stations.end()) {
        return;
    }
    
    std::pair<int, int> key = makeRoutePair(id1, id2);
    
    if (routes.find(key) != routes.end()) {
        scene->removeItem(routes[key]);
        delete routes[key];
        routes.erase(key);
    }
    
    if (routeLabels.find(key) != routeLabels.end()) {
        scene->removeItem(routeLabels[key]);
        delete routeLabels[key];
        routeLabels.erase(key);
    }
    
    DraggableStation* station1 = stations[id1];
    DraggableStation* station2 = stations[id2];
    
    QRectF bounds1 = station1->rect();
    QRectF bounds2 = station2->rect();
    
    QPointF center1 = bounds1.center() + station1->pos();
    QPointF center2 = bounds2.center() + station2->pos();
    
    ClickableRoute* route = new ClickableRoute(id1, id2, QLineF(center1, center2), mainWindow);
    scene->addItem(route);
    route->setZValue(-1);
    
    routes[key] = route;
    
    QGraphicsTextItem* weightLabel = new QGraphicsTextItem(QString::number(travelTime));
    weightLabel->setDefaultTextColor(QColor(156, 163, 175));
    weightLabel->setFont(QFont("Arial", 10, QFont::Bold));
    QPointF midPoint = (center1 + center2) / 2.0;
    weightLabel->setPos(midPoint.x() - weightLabel->boundingRect().width() / 2,
                        midPoint.y() + 5);
    scene->addItem(weightLabel);
    weightLabel->setZValue(0);
    routeLabels[key] = weightLabel;
    
    graph->addRoute(id1, id2, travelTime);
}

void NetworkManager::deleteRoute(int id1, int id2) {
    std::pair<int, int> key = makeRoutePair(id1, id2);
    
    if (routes.find(key) == routes.end()) {
        return;
    }
    
    ClickableRoute* route = routes[key];
    scene->removeItem(route);
    delete route;
    
    if (routeLabels.find(key) != routeLabels.end()) {
        QGraphicsTextItem* label = routeLabels[key];
        scene->removeItem(label);
        delete label;
        routeLabels.erase(key);
    }
    
    routes.erase(key);
    graph->removeRoute(id1, id2);
}

void NetworkManager::updateRoutePosition(int stationId, const QPointF& center) {
    if (stations.find(stationId) == stations.end()) {
        return;
    }
    
    std::vector<int> neighbors = graph->getConnectedStations(stationId);
    
    for (int neighborId : neighbors) {
        if (stations.find(neighborId) == stations.end()) {
            continue;
        }
        
        DraggableStation* neighbor = stations[neighborId];
        QRectF neighborBounds = neighbor->rect();
        QPointF neighborCenter = neighborBounds.center() + neighbor->pos();
        
        std::pair<int, int> key = makeRoutePair(stationId, neighborId);
        if (routes.find(key) != routes.end()) {
            ClickableRoute* route = routes[key];
            route->setLine(QLineF(center, neighborCenter));
            
            if (routeLabels.find(key) != routeLabels.end()) {
                QGraphicsTextItem* label = routeLabels[key];
                QPointF midPoint = (center + neighborCenter) / 2.0;
                label->setPos(midPoint.x() - label->boundingRect().width() / 2,
                             midPoint.y() + 5);
            }
        }
    }
    
    tree->updatePosition(stationId, center.x(), center.y());
}

void NetworkManager::updateRouteVisualState(int id1, int id2) {
    std::pair<int, int> key = makeRoutePair(id1, id2);
    
    if (routes.find(key) != routes.end()) {
        ClickableRoute* route = routes[key];
        bool isBlocked = graph->isRouteBlocked(id1, id2);
        route->setBlocked(isBlocked);
    }
}

void NetworkManager::clearAll() {
    scene->clear();
    stations.clear();
    routes.clear();
    routeLabels.clear();
    tree->clear();
    graph->clear();
}

void NetworkManager::reconstructFromData(StationTree* sourceTree, TransportGraph* sourceGraph) {
    std::vector<StationNode*> stationNodes = sourceTree->getAllStations();
    
    for (StationNode* node : stationNodes) {
        DraggableStation* station = new DraggableStation(node->id, node->x, node->y, 32, mainWindow);
        scene->addItem(station);
        
        QGraphicsTextItem* labelItem = new QGraphicsTextItem(QString::fromStdString(node->name));
        labelItem->setDefaultTextColor(QColor(255, 255, 255));
        scene->addItem(labelItem);
        
        QRectF bounds = station->rect();
        QPointF center = bounds.center() + station->pos();
        labelItem->setPos(center.x() - labelItem->boundingRect().width() / 2,
                          center.y() + bounds.height() / 2);
        
        station->setLabel(labelItem);
        stations[node->id] = station;
    }
    
    std::vector<int> allStationIds = sourceGraph->getAllStations();
    for (int stationId : allStationIds) {
        std::vector<int> neighbors = sourceGraph->getConnectedStations(stationId);
        for (int neighbor : neighbors) {
            if (stationId < neighbor) {
                std::pair<int, int> key = makeRoutePair(stationId, neighbor);
                if (routes.find(key) == routes.end()) {
                    DraggableStation* station1 = stations[stationId];
                    DraggableStation* station2 = stations[neighbor];
                    
                    QRectF bounds1 = station1->rect();
                    QRectF bounds2 = station2->rect();
                    
                    QPointF center1 = bounds1.center() + station1->pos();
                    QPointF center2 = bounds2.center() + station2->pos();
                    
                    QLineF line(center1, center2);
                    ClickableRoute* route = new ClickableRoute(stationId, neighbor, line, mainWindow);
                    scene->addItem(route);
                    route->setZValue(-1);
                    routes[key] = route;
                    
                    bool isBlocked = sourceGraph->isRouteBlocked(stationId, neighbor);
                    route->setBlocked(isBlocked);
                    
                    int weight = sourceGraph->getRouteTime(stationId, neighbor);
                    QGraphicsTextItem* weightLabel = new QGraphicsTextItem(QString::number(weight));
                    weightLabel->setDefaultTextColor(QColor(156, 163, 175));
                    weightLabel->setFont(QFont("Arial", 10, QFont::Bold));
                    QPointF midPoint = (center1 + center2) / 2.0;
                    weightLabel->setPos(midPoint.x() - weightLabel->boundingRect().width() / 2,
                                        midPoint.y() + 5);
                    scene->addItem(weightLabel);
                    weightLabel->setZValue(0);
                    routeLabels[key] = weightLabel;
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
    if (stations.find(id) != stations.end()) {
        return stations[id];
    }
    return nullptr;
}

std::vector<StationNode*> NetworkManager::getAllStations() {
    return tree->getAllStations();
}

std::map<int, DraggableStation*>& NetworkManager::getStationItems() {
    return stations;
}

std::map<std::pair<int, int>, ClickableRoute*>& NetworkManager::getRouteItems() {
    return routes;
}

void NetworkManager::highlightPath(const std::vector<int>& path) {
    clearHighlights();
    
    for (int stationId : path) {
        if (stations.find(stationId) != stations.end()) {
            stations[stationId]->setBrush(QBrush(QColor(250, 204, 21)));
        }
    }
    
    for (size_t i = 0; i < path.size() - 1; i++) {
        std::pair<int, int> key = makeRoutePair(path[i], path[i + 1]);
        if (routes.find(key) != routes.end()) {
            routes[key]->setPen(QPen(QColor(250, 204, 21), 5));
        }
    }
}

void NetworkManager::clearHighlights() {
    for (auto& pair : stations) {
        pair.second->setBrush(QBrush(QColor(124, 58, 237)));
    }
    
    for (auto& pair : routes) {
        pair.second->setPen(QPen(QColor(107, 114, 128), 2));
    }
}

std::pair<int, int> NetworkManager::makeRoutePair(int a, int b) {
    if (a < b) {
        return make_pair(a, b);
    }
    return make_pair(b, a);
}
