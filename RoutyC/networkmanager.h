#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <map>
#include <utility>
#include <vector>
#include <QString>
#include <QPointF>
#include "stationtree.h"
#include "transportgraph.h"

using namespace std;

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
    void updateRouteVisualState(int id1, int id2);
    void clearAll();
    void reconstructFromData(StationTree* tree, TransportGraph* graph);
    
    StationTree* getTree();
    TransportGraph* getGraph();
    DraggableStation* getStation(int id);
    vector<StationNode*> getAllStations();
    map<int, DraggableStation*>& getStationItems();
    map<pair<int, int>, ClickableRoute*>& getRouteItems();
    
    void highlightPath(const vector<int>& path);
    void clearHighlights();
    
private:
    QGraphicsScene* scene;
    MainWindow* mainWindow;
    StationTree* tree;
    TransportGraph* graph;
    
    map<int, DraggableStation*> stations;
    map<pair<int, int>, ClickableRoute*> routes;
    map<pair<int, int>, QGraphicsTextItem*> routeLabels;
    
    pair<int, int> makeRoutePair(int a, int b);
};

#endif
