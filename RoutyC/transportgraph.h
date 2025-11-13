#ifndef TRANSPORTGRAPH_H
#define TRANSPORTGRAPH_H

#include <map>
#include <vector>
#include <set>

using namespace std;

struct Edge {
    int destination;
    int weight;
    
    Edge(int dest, int w);
};

class TransportGraph {
public:
    TransportGraph();
    ~TransportGraph();
    
    void addStation(int id);
    void removeStation(int id);
    void addRoute(int source, int destination, int weight);
    void removeRoute(int source, int destination);
    bool hasRoute(int source, int destination);
    int getRouteTime(int source, int destination);
    vector<int> getConnectedStations(int stationId);
    vector<int> getAllConnectedStations(int stationId);
    vector<int> getAllStations();
    void clear();
    bool canReach(int source, int destination);
    bool isFullyConnected();
    void blockRoute(int source, int destination);
    void unblockRoute(int source, int destination);
    void clearBlockedRoutes();
    bool isRouteBlocked(int source, int destination);
    vector<pair<int, int>> getBlockedRoutes();
    
private:
    map<int, vector<Edge*>> connections;
    set<pair<int, int>> blockedRoutes;
    
    void removeOneWayRoute(int source, int destination);
    pair<int, int> makeRoutePair(int a, int b);
};

#endif
