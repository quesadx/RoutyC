#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filemanager.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QTextStream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

DraggableStation::DraggableStation(int stationId, double x, double y, double diameter)
    : QGraphicsEllipseItem(x - diameter/2, y - diameter/2, diameter, diameter),
      stationId(stationId), label(nullptr) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setBrush(QBrush(Qt::blue));
    setPen(QPen(Qt::black, 2));
}

int DraggableStation::getStationId() const {
    return stationId;
}

QGraphicsTextItem* DraggableStation::getLabel() {
    return label;
}

void DraggableStation::setLabel(QGraphicsTextItem* textItem) {
    label = textItem;
}

void DraggableStation::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    MainWindow* mainWindow = nullptr;
    if (scene() && scene()->views().size() > 0) {
        QGraphicsView* view = scene()->views().at(0);
        mainWindow = qobject_cast<MainWindow*>(view->window());
    }
    
    if (mainWindow) {
        mainWindow->handleStationClick(this);
    }
    
    QGraphicsEllipseItem::mousePressEvent(event);
}

void DraggableStation::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsEllipseItem::mouseMoveEvent(event);
    
    if (label) {
        QRectF bounds = rect();
        label->setPos(bounds.center().x() + pos().x() - label->boundingRect().width() / 2,
                      bounds.center().y() + pos().y() + bounds.height() / 2);
    }
}

void DraggableStation::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsEllipseItem::mouseReleaseEvent(event);
    
    MainWindow* mainWindow = nullptr;
    if (scene() && scene()->views().size() > 0) {
        QGraphicsView* view = scene()->views().at(0);
        mainWindow = qobject_cast<MainWindow*>(view->window());
    }
    
    if (mainWindow) {
        QRectF bounds = rect();
        QPointF center = bounds.center() + pos();
        mainWindow->handleStationMoved(stationId, center);
    }
}

void DraggableStation::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QMenu menu;
    QAction* deleteAction = menu.addAction("Delete Station");
    
    QAction* selected = menu.exec(event->screenPos());
    if (selected == deleteAction) {
        MainWindow* mainWindow = nullptr;
        if (scene() && scene()->views().size() > 0) {
            QGraphicsView* view = scene()->views().at(0);
            mainWindow = qobject_cast<MainWindow*>(view->window());
        }
        
        if (mainWindow) {
            mainWindow->handleStationDelete(stationId);
        }
    }
}

ClickableRoute::ClickableRoute(int id1, int id2, const QLineF& line)
    : QGraphicsLineItem(line), sourceId(id1), destId(id2) {
    setFlag(QGraphicsItem::ItemIsSelectable);
    setPen(QPen(Qt::black, 2));
}

int ClickableRoute::getSourceId() const {
    return sourceId;
}

int ClickableRoute::getDestId() const {
    return destId;
}

void ClickableRoute::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QMenu menu;
    QAction* deleteAction = menu.addAction("Delete Route");
    
    QAction* selected = menu.exec(event->screenPos());
    if (selected == deleteAction) {
        MainWindow* mainWindow = nullptr;
        if (scene() && scene()->views().size() > 0) {
            QGraphicsView* view = scene()->views().at(0);
            mainWindow = qobject_cast<MainWindow*>(view->window());
        }
        
        if (mainWindow) {
            mainWindow->handleRouteDelete(sourceId, destId);
        }
    }
}

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent), ui(new Ui::MainWindow), 
      scene(nullptr), tree(nullptr), graph(nullptr),
      nextStationId(101), selectedStationId(-1) {
    ui->setupUi(this);
    setupScene();
    setupAlgorithms();
}

MainWindow::~MainWindow() {
    if (tree) {
        delete tree;
    }
    if (graph) {
        delete graph;
    }
    for (PathAlgorithm* algo : algorithms) {
        delete algo;
    }
    if (scene) {
        delete scene;
void MainWindow::setupScene() {
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    ui->gvArea->setScene(scene);
    
    tree = new StationTree();
    graph = new TransportGraph();
    
    ui->gvArea->viewport()->installEventFilter(this);
    ui->gvArea->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::setupAlgorithms() {
    algorithms.push_back(new DijkstraAlgorithm());
    algorithms.push_back(new BFSAlgorithm());
    algorithms.push_back(new DFSAlgorithm());
    
    for (PathAlgorithm* algo : algorithms) {
        ui->cbAlgorithm->addItem(QString::fromStdString(algo->getName()));
    }
}   graph = new TransportGraph();
    
    ui->gvArea->viewport()->installEventFilter(this);
    ui->gvArea->setRenderHint(QPainter::Antialiasing);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == ui->gvArea->viewport() && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = ui->gvArea->mapToScene(mouseEvent->pos());
        
        QGraphicsItem* item = scene->itemAt(scenePos, ui->gvArea->transform());
        if (!item) {
            bool ok;
            QString name = QInputDialog::getText(this, "Create Station", "Enter station name:", 
                                                 QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                createStation(name, scenePos.x(), scenePos.y());
            }
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::createStation(const QString& name, double x, double y) {
    int id = nextStationId++;
    
    DraggableStation* station = new DraggableStation(id, x, y, 40);
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
    
    updateComboBoxes();
}

void MainWindow::deleteStation(int stationId) {
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
    
    if (selectedStationId == stationId) {
        selectedStationId = -1;
    }
    
    updateComboBoxes();
}

void MainWindow::createRoute(int id1, int id2, int travelTime) {
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
    
    ClickableRoute* route = new ClickableRoute(id1, id2, QLineF(center1, center2));
    scene->addItem(route);
    route->setZValue(-1);
    
    std::pair<int, int> key = normalizeEdgePair(id1, id2);
    routeItems[key] = route;
    
    graph->addEdge(id1, id2, travelTime);
}

void MainWindow::deleteRoute(int id1, int id2) {
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

void MainWindow::updateRoutePositions(int stationId) {
    if (stationItems.find(stationId) == stationItems.end()) {
        return;
    }
    
    DraggableStation* station = stationItems[stationId];
    QRectF bounds = station->rect();
    QPointF center = bounds.center() + station->pos();
    
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
}

void MainWindow::updateComboBoxes() {
    QString currentOrigin = ui->cbOrigin->currentText();
    QString currentDest = ui->cbDestination->currentText();
    
    ui->cbOrigin->clear();
    ui->cbDestination->clear();
    
    std::vector<StationNode*> stations = tree->getAllStations();
    for (StationNode* node : stations) {
        QString stationName = QString::fromStdString(node->name);
        ui->cbOrigin->addItem(stationName, node->id);
        ui->cbDestination->addItem(stationName, node->id);
    }
    
    int originIndex = ui->cbOrigin->findText(currentOrigin);
    if (originIndex >= 0) {
        ui->cbOrigin->setCurrentIndex(originIndex);
    }
    
    int destIndex = ui->cbDestination->findText(currentDest);
    if (destIndex >= 0) {
        ui->cbDestination->setCurrentIndex(destIndex);
    }
}

std::pair<int, int> MainWindow::normalizeEdgePair(int a, int b) {
    if (a < b) {
        return std::make_pair(a, b);
    }
    return std::make_pair(b, a);
}

void MainWindow::handleSceneMouseDoubleClick(QGraphicsSceneMouseEvent* event) {
    QPointF pos = event->scenePos();
    
    QGraphicsItem* item = scene->itemAt(pos, QTransform());
    if (item) {
        return;
    }
    
    bool ok;
    QString name = QInputDialog::getText(this, "Create Station", "Enter station name:", 
                                         QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        createStation(name, pos.x(), pos.y());
    }
}

void MainWindow::handleStationClick(DraggableStation* station) {
    int clickedId = station->getStationId();
    
    if (selectedStationId == -1) {
        selectedStationId = clickedId;
        station->setBrush(QBrush(Qt::green));
    } else if (selectedStationId == clickedId) {
        selectedStationId = -1;
        station->setBrush(QBrush(Qt::blue));
    } else {
        int firstId = selectedStationId;
        int secondId = clickedId;
        
        if (stationItems.find(firstId) != stationItems.end()) {
            stationItems[firstId]->setBrush(QBrush(Qt::blue));
        }
        
        bool ok;
        int travelTime = QInputDialog::getInt(this, "Create Route", 
                                              "Enter travel time (minutes):", 
                                              10, 1, 1000, 1, &ok);
        if (ok) {
            createRoute(firstId, secondId, travelTime);
        }
        
        selectedStationId = -1;
    }
}
void MainWindow::on_pbCalculateWithAlgorithm_clicked() {
    int originIndex = ui->cbOrigin->currentIndex();
    int destIndex = ui->cbDestination->currentIndex();
    int algoIndex = ui->cbAlgorithm->currentIndex();
    
    if (originIndex < 0 || destIndex < 0 || algoIndex < 0) {
        QMessageBox::warning(this, "Error", "Please select origin, destination, and algorithm");
        return;
    }
    
    int originId = ui->cbOrigin->itemData(originIndex).toInt();
    int destId = ui->cbDestination->itemData(destIndex).toInt();
    
    if (originId == destId) {
        QMessageBox::warning(this, "Error", "Origin and destination must be different");
        return;
void MainWindow::on_actionSave_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, "Save Network", "", "RoutyC Files (*.rty);;All Files (*)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    if (FileManager::saveToFile(filename.toStdString(), tree, graph)) {
        QMessageBox::information(this, "Success", "Network saved successfully");
    } else {
        QMessageBox::critical(this, "Error", "Failed to save network");
    }
}

void MainWindow::on_actionLoad_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, "Load Network", "", "RoutyC Files (*.rty);;All Files (*)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    scene->clear();
    stationItems.clear();
    routeItems.clear();
    selectedStationId = -1;
    
    if (FileManager::loadFromFile(filename.toStdString(), tree, graph)) {
        reconstructGraphFromData();
        updateComboBoxes();
        
        std::vector<StationNode*> stations = tree->getAllStations();
        if (!stations.empty()) {
            int maxId = 0;
            for (StationNode* node : stations) {
                if (node->id > maxId) {
                    maxId = node->id;
                }
            }
            nextStationId = maxId + 1;
void MainWindow::on_actionExportTraversals_triggered() {
    exportTraversals();
}

void MainWindow::on_actionGenerateReport_triggered() {
    generateReport();
}   
    ui->pteOutput->clear();
    ui->pteOutput->appendPlainText("Algorithm: " + QString::fromStdString(result.algorithmName));
    ui->pteOutput->appendPlainText("Origin: " + ui->cbOrigin->currentText());
    ui->pteOutput->appendPlainText("Destination: " + ui->cbDestination->currentText());
    ui->pteOutput->appendPlainText("");
    
    if (result.found) {
        highlightPath(result.path);
        
        ui->pteOutput->appendPlainText("Path found!");
        ui->pteOutput->appendPlainText("Total cost: " + QString::number(result.totalCost) + " minutes");
        ui->pteOutput->appendPlainText("");
        
        QString pathStr = "Path: ";
        for (size_t i = 0; i < result.path.size(); i++) {
            StationNode* node = tree->findStation(result.path[i]);
            if (node) {
                pathStr += QString::fromStdString(node->name);
                if (i < result.path.size() - 1) {
                    pathStr += " -> ";
                }
            }
        }
        ui->pteOutput->appendPlainText(pathStr);
        ui->pteOutput->appendPlainText("");
        ui->pteOutput->appendPlainText("Execution steps:");
        
        for (const std::string& step : result.steps) {
            ui->pteOutput->appendPlainText(QString::fromStdString(step));
        }
    } else {
        ui->pteOutput->appendPlainText("No path found!");
        ui->pteOutput->appendPlainText("");
        ui->pteOutput->appendPlainText("Execution steps:");
        
        for (const std::string& step : result.steps) {
            ui->pteOutput->appendPlainText(QString::fromStdString(step));
        }
    }
}   tree->updatePosition(stationId, newPos.x(), newPos.y());
    updateRoutePositions(stationId);
}

void MainWindow::handleStationDelete(int stationId) {
    deleteStation(stationId);
}

void MainWindow::handleRouteDelete(int sourceId, int destId) {
    deleteRoute(sourceId, destId);
}

void MainWindow::on_pbCalculateWithAlgorithm_clicked() {
    std::cout << "pbCalculateWithAlgorithm clicked!" << std::endl;
}

void MainWindow::on_cbOrigin_currentIndexChanged(int index) {
    std::cout << "cbOrigin changed to index: " << index << std::endl;
}

void MainWindow::on_cbDestination_currentIndexChanged(int index) {
    std::cout << "cbDestination changed to index: " << index << std::endl;
}

void MainWindow::on_cbAlgorithm_currentIndexChanged(int index) {
    std::cout << "cbAlgorithm changed to index: " << index << std::endl;
}

void MainWindow::on_actionSave_triggered() {
    std::cout << "actionSave clicked!" << std::endl;
}

void MainWindow::on_actionLoad_triggered() {
    std::cout << "actionLoad clicked!" << std::endl;
}

void MainWindow::on_actionClearCurrentDisplay_triggered() {
    scene->clear();
    stationItems.clear();
    routeItems.clear();
    tree->clear();
    graph->clear();
    selectedStationId = -1;
    nextStationId = 101;
    updateComboBoxes();
}

void MainWindow::on_actionCreateRoute_triggered() {
    QMessageBox::information(this, "Create Route", 
                           "Click on two stations in the map to create a route.");
}

void MainWindow::on_actionDeleteRoute_triggered() {
    QMessageBox::information(this, "Delete Route", 
                           "Right-click on a route line to delete it.");
}

void MainWindow::on_actionCreateStation_triggered() {
    QMessageBox::information(this, "Create Station", 
                           "Double-click on an empty area to create a station.");
}

void MainWindow::on_actionDeleteStation_triggered() {
    QMessageBox::information(this, "Delete Station", 
                           "Right-click on a station to delete it.");
}

void MainWindow::on_actionExportTraversals_triggered() {
    std::cout << "actionExportTraversals clicked!" << std::endl;
}

void MainWindow::on_actionGenerateReport_triggered() {
    std::cout << "actionGenerateReport clicked!" << std::endl;
}
