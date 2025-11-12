#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <map>
#include <utility>
#include <vector>
#include "stationtree.h"
#include "transportgraph.h"
#include "pathalgorithm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DraggableStation : public QGraphicsEllipseItem {
public:
    DraggableStation(int stationId, double x, double y, double diameter);
    int getStationId() const;
    QGraphicsTextItem* getLabel();
    void setLabel(QGraphicsTextItem* textItem);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    
private:
    int stationId;
    QGraphicsTextItem* label;
};

class ClickableRoute : public QGraphicsLineItem {
public:
    ClickableRoute(int id1, int id2, const QLineF& line);
    int getSourceId() const;
    int getDestId() const;
    
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    
private:
    int sourceId;
    int destId;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    bool eventFilter(QObject* obj, QEvent* event) override;
    
    void handleSceneMouseDoubleClick(QGraphicsSceneMouseEvent* event);
    void handleStationClick(DraggableStation* station);
    void handleStationMoved(int stationId, QPointF newPos);
    void handleStationDelete(int stationId);
    void handleRouteDelete(int sourceId, int destId);

private slots:
    void on_pbCalculateWithAlgorithm_clicked();
    void on_cbOrigin_currentIndexChanged(int index);
    void on_cbDestination_currentIndexChanged(int index);
    void on_cbAlgorithm_currentIndexChanged(int index);
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void on_actionClearCurrentDisplay_triggered();
    void on_actionCreateRoute_triggered();
    void on_actionDeleteRoute_triggered();
    void on_actionCreateStation_triggered();
    void on_actionDeleteStation_triggered();
    void on_actionExportTraversals_triggered();
    void on_actionGenerateReport_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    StationTree* tree;
    TransportGraph* graph;
    
    std::map<int, DraggableStation*> stationItems;
    std::map<std::pair<int, int>, ClickableRoute*> routeItems;
    std::vector<PathAlgorithm*> algorithms;
    
    int nextStationId;
    int selectedStationId;
    
    void setupScene();
    void setupAlgorithms();
    void createStation(const QString& name, double x, double y);
    void deleteStation(int stationId);
    void createRoute(int id1, int id2, int travelTime);
    void deleteRoute(int id1, int id2);
    void updateRoutePositions(int stationId);
    void updateComboBoxes();
    void highlightPath(const std::vector<int>& path);
    void clearHighlights();
    void reconstructGraphFromData();
    void exportTraversals();
    void generateReport();
    std::pair<int, int> normalizeEdgePair(int a, int b);
};

#endif
