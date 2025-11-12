#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <vector>
#include "networkmanager.h"
#include "pathalgorithm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DraggableStation;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    bool eventFilter(QObject* obj, QEvent* event) override;
    
    void handleStationClick(DraggableStation* station);
    void handleStationMoved(int stationId, QPointF newPos);
    void handleStationDelete(int stationId);
    void handleRouteDelete(int sourceId, int destId);
    void updateRoutePositionsDuringDrag(int stationId, const QPointF& center);

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
    NetworkManager* networkManager;
    std::vector<PathAlgorithm*> algorithms;
    
    int nextStationId;
    int selectedStationId;
    
    void setupScene();
    void setupAlgorithms();
    void createStation(const QString& name, double x, double y);
    void updateComboBoxes();
    void exportTraversals();
    void generateReport();
};

#endif
