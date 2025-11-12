#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <map>
#include <utility>
#include <vector>
#include <QString>
#include <QPointF>
#include "stationtree.h"
#include "transportgraph.h"

class DraggableStation;
class ClickableRoute;
class QGraphicsScene;
class QGraphicsTextItem;
class MainWindow;

class NetworkManager {
public:
    NetworkManager(QGraphicsScene* scene, MainWindow* window);
    ~NetworkManager();
    
    void createStation(int id, const QString& name, double x, double y);
    void deleteStation(int stationId);
    void createRoute(int id1, int id2, int travelTime);
    void deleteRoute(int id1, int id2);
    void updateRoutePosition(int stationId, const QPointF& center);
    void clearAll();
    void reconstructFromData(StationTree* tree, TransportGraph* graph);
    
    StationTree* getTree();
    TransportGraph* getGraph();
    DraggableStation* getStation(int id);
    std::vector<StationNode*> getAllStations();
    std::map<int, DraggableStation*>& getStationItems();
    std::map<std::pair<int, int>, ClickableRoute*>& getRouteItems();
    
    void highlightPath(const std::vector<int>& path);
    void clearHighlights();
    
private:
    QGraphicsScene* scene;
    MainWindow* mainWindow;
    StationTree* tree;
    TransportGraph* graph;
    
    std::map<int, DraggableStation*> stationItems;
    std::map<std::pair<int, int>, ClickableRoute*> routeItems;
    std::map<std::pair<int, int>, QGraphicsTextItem*> routeWeightLabels;
    
    std::pair<int, int> normalizeEdgePair(int a, int b);
};

#endif
