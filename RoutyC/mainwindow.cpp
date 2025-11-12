#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsitems.h"
#include "filemanager.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QBrush>

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent), ui(new Ui::MainWindow), 
      scene(nullptr), networkManager(nullptr),
      nextStationId(101), selectedStationId(-1) {
    ui->setupUi(this);
    statusBar()->hide();
    setupScene();
    setupAlgorithms();
    updateGeneralInfo();
}

MainWindow::~MainWindow() {
    if (networkManager) {
        delete networkManager;
    }
    for (PathAlgorithm* algo : algorithms) {
        delete algo;
    }
    if (scene) {
        delete scene;
    }
    delete ui;
}

void MainWindow::setupScene() {
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    ui->gvArea->setScene(scene);
    
    networkManager = new NetworkManager(scene, this);
    
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
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == ui->gvArea->viewport() && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = ui->gvArea->mapToScene(mouseEvent->pos());
        
        QGraphicsItem* item = scene->itemAt(scenePos, ui->gvArea->transform());
        if (!item) {
            bool ok;
            QString name = QInputDialog::getText(this, "Crear Estación", "Ingrese el nombre de la estación:", 
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
    networkManager->createStation(id, name, x, y);
    updateComboBoxes();
    updateGeneralInfo();
}

void MainWindow::updateComboBoxes() {
    QString currentOrigin = ui->cbOrigin->currentText();
    QString currentDest = ui->cbDestination->currentText();
    
    ui->cbOrigin->clear();
    ui->cbDestination->clear();
    
    std::vector<StationNode*> stations = networkManager->getAllStations();
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

void MainWindow::updateGeneralInfo() {
    std::vector<StationNode*> stations = networkManager->getAllStations();
    int totalStations = stations.size();
    
    TransportGraph* graph = networkManager->getGraph();
    int totalRoutes = 0;
    std::vector<int> allStations = graph->getAllStations();
    for (int stationId : allStations) {
        totalRoutes += graph->getNeighbors(stationId).size();
    }
    totalRoutes /= 2;
    
    QString info = QString("Estaciones totales: %1 | Rutas totales: %2")
                   .arg(totalStations)
                   .arg(totalRoutes);
    ui->lblGeneralInfo->setText(info);
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
        
        DraggableStation* firstStation = networkManager->getStation(firstId);
        if (firstStation) {
            firstStation->setBrush(QBrush(Qt::blue));
        }
        
        bool ok;
        int travelTime = QInputDialog::getInt(this, "Crear Ruta", 
                                              "Ingrese el tiempo de viaje (minutos):", 
                                              10, 1, 1000, 1, &ok);
        if (ok) {
            networkManager->createRoute(firstId, secondId, travelTime);
            updateGeneralInfo();
        }
        
        selectedStationId = -1;
    }
}

void MainWindow::handleStationMoved(int stationId, QPointF newPos) {
    networkManager->updateRoutePosition(stationId, newPos);
}

void MainWindow::handleStationDelete(int stationId) {
    networkManager->deleteStation(stationId);
    
    if (selectedStationId == stationId) {
        selectedStationId = -1;
    }
    
    updateComboBoxes();
    updateGeneralInfo();
}

void MainWindow::handleRouteDelete(int sourceId, int destId) {
    networkManager->deleteRoute(sourceId, destId);
    updateGeneralInfo();
}

void MainWindow::updateRoutePositionsDuringDrag(int stationId, const QPointF& center) {
    networkManager->updateRoutePosition(stationId, center);
}

void MainWindow::exportTraversals() {
    QString filename = QFileDialog::getSaveFileName(this, "Exportar Recorridos", "", 
                                                    "Archivos de Texto (*.txt);;Todos los Archivos (*)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo para escritura");
        return;
    }
    
    QTextStream out(&file);
    
    out << "=== STATION TREE TRAVERSALS ===\n\n";
    out << "In-order Traversal (BST):\n";
    
    std::vector<StationNode*> stations = networkManager->getAllStations();
    for (StationNode* node : stations) {
        out << "  ID: " << node->id << " - " << QString::fromStdString(node->name) << "\n";
    }
    out << "\n";
    
    out << "=== TRANSPORT GRAPH ===\n\n";
    
    TransportGraph* graph = networkManager->getGraph();
    std::vector<int> allStations = graph->getAllStations();
    
    for (int stationId : allStations) {
        StationNode* node = networkManager->getTree()->findStation(stationId);
        if (node) {
            out << "Station " << node->id << " (" << QString::fromStdString(node->name) << "):\n";
            std::vector<int> neighbors = graph->getNeighbors(stationId);
            for (int neighbor : neighbors) {
                StationNode* neighborNode = networkManager->getTree()->findStation(neighbor);
                if (neighborNode) {
                    int weight = graph->getEdgeWeight(stationId, neighbor);
                    out << "  -> " << neighborNode->id << " (" 
                        << QString::fromStdString(neighborNode->name) << ") - " << weight << " min\n";
                }
            }
            out << "\n";
        }
    }
    
    file.close();
    QMessageBox::information(this, "Éxito", "Recorridos exportados exitosamente");
}

void MainWindow::generateReport() {
    QString filename = QFileDialog::getSaveFileName(this, "Generar Reporte", "", 
                                                    "Archivos de Texto (*.txt);;Todos los Archivos (*)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo para escritura");
        return;
    }
    
    QTextStream out(&file);
    
    out << "======================================\n";
    out << "    TRANSPORT NETWORK REPORT\n";
    out << "======================================\n\n";
    
    std::vector<StationNode*> stations = networkManager->getAllStations();
    out << "Total Stations: " << stations.size() << "\n\n";
    
    TransportGraph* graph = networkManager->getGraph();
    int totalRoutes = 0;
    std::vector<int> allStations = graph->getAllStations();
    for (int stationId : allStations) {
        totalRoutes += graph->getNeighbors(stationId).size();
    }
    totalRoutes /= 2;
    out << "Total Routes: " << totalRoutes << "\n\n";
    
    out << "--- STATIONS ---\n";
    for (StationNode* node : stations) {
        out << "ID: " << node->id << "\n";
        out << "Name: " << QString::fromStdString(node->name) << "\n";
        out << "Position: (" << node->x << ", " << node->y << ")\n";
        
        std::vector<int> neighbors = graph->getNeighbors(node->id);
        out << "Connections: " << neighbors.size() << "\n";
        
        for (int neighbor : neighbors) {
            StationNode* neighborNode = networkManager->getTree()->findStation(neighbor);
            if (neighborNode) {
                int weight = graph->getEdgeWeight(node->id, neighbor);
                out << "  -> " << QString::fromStdString(neighborNode->name) 
                    << " (" << weight << " min)\n";
            }
        }
        out << "\n";
    }
    
    out << "--- CONNECTIVITY ANALYSIS ---\n";
    for (StationNode* node : stations) {
        int connections = graph->getNeighbors(node->id).size();
        out << QString::fromStdString(node->name) << ": " << connections << " connection";
        if (connections != 1) out << "s";
        out << "\n";
    }
    
    file.close();
    QMessageBox::information(this, "Éxito", "Reporte generado exitosamente");
}

void MainWindow::on_pbCalculateWithAlgorithm_clicked() {
    int originIndex = ui->cbOrigin->currentIndex();
    int destIndex = ui->cbDestination->currentIndex();
    int algoIndex = ui->cbAlgorithm->currentIndex();
    
    if (originIndex < 0 || destIndex < 0 || algoIndex < 0) {
        QMessageBox::warning(this, "Error", "Por favor seleccione origen, destino y algoritmo");
        return;
    }
    
    int originId = ui->cbOrigin->itemData(originIndex).toInt();
    int destId = ui->cbDestination->itemData(destIndex).toInt();
    
    if (originId == destId) {
        QMessageBox::warning(this, "Error", "El origen y el destino deben ser diferentes");
        return;
    }
    
    networkManager->clearHighlights();
    
    PathAlgorithm* algo = algorithms[algoIndex];
    PathResult result = algo->findPath(networkManager->getGraph(), originId, destId);
    
    ui->pteOutput->clear();
    ui->pteOutput->appendPlainText("Algoritmo: " + QString::fromStdString(result.algorithmName));
    ui->pteOutput->appendPlainText("Origen: " + ui->cbOrigin->currentText());
    ui->pteOutput->appendPlainText("Destino: " + ui->cbDestination->currentText());
    ui->pteOutput->appendPlainText("");
    
    if (result.found) {
        networkManager->highlightPath(result.path);
        
        ui->pteOutput->appendPlainText("¡Ruta encontrada!");
        ui->pteOutput->appendPlainText("Costo total: " + QString::number(result.totalCost) + " minutos");
        ui->pteOutput->appendPlainText("");
        
        QString pathStr = "Ruta: ";
        for (size_t i = 0; i < result.path.size(); i++) {
            StationNode* node = networkManager->getTree()->findStation(result.path[i]);
            if (node) {
                pathStr += QString::fromStdString(node->name);
                if (i < result.path.size() - 1) {
                    pathStr += " -> ";
                }
            }
        }
        ui->pteOutput->appendPlainText(pathStr);
        ui->pteOutput->appendPlainText("");
        ui->pteOutput->appendPlainText("Pasos de ejecución:");
        
        for (const std::string& step : result.steps) {
            ui->pteOutput->appendPlainText(QString::fromStdString(step));
        }
    } else {
        ui->pteOutput->appendPlainText("¡No se encontró una ruta!");
        ui->pteOutput->appendPlainText("");
        ui->pteOutput->appendPlainText("Pasos de ejecución:");
        
        for (const std::string& step : result.steps) {
            ui->pteOutput->appendPlainText(QString::fromStdString(step));
        }
    }
}

void MainWindow::on_cbOrigin_currentIndexChanged(int index) {
}

void MainWindow::on_cbDestination_currentIndexChanged(int index) {
}

void MainWindow::on_cbAlgorithm_currentIndexChanged(int index) {
}

void MainWindow::on_actionSave_triggered() {
    QString filename = QFileDialog::getSaveFileName(this, "Guardar Red", "", 
                                                    "Archivos RoutyC (*.rty);;Todos los Archivos (*)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    if (FileManager::saveToFile(filename.toStdString(), networkManager->getTree(), 
                                networkManager->getGraph())) {
        QMessageBox::information(this, "Éxito", "Red guardada exitosamente");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo guardar la red");
    }
}

void MainWindow::on_actionLoad_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, "Cargar Red", "", 
                                                    "Archivos RoutyC (*.rty);;Todos los Archivos (*)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    networkManager->clearAll();
    selectedStationId = -1;
    
    if (FileManager::loadFromFile(filename.toStdString(), networkManager->getTree(), 
                                  networkManager->getGraph())) {
        networkManager->reconstructFromData(networkManager->getTree(), networkManager->getGraph());
        updateComboBoxes();
        
        std::vector<StationNode*> stations = networkManager->getAllStations();
        if (!stations.empty()) {
            int maxId = 0;
            for (StationNode* node : stations) {
                if (node->id > maxId) {
                    maxId = node->id;
                }
            }
            nextStationId = maxId + 1;
        }
        
        updateGeneralInfo();
        QMessageBox::information(this, "Éxito", "Red cargada exitosamente");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo cargar la red");
    }
}

void MainWindow::on_actionClearCurrentDisplay_triggered() {
    networkManager->clearAll();
    selectedStationId = -1;
    nextStationId = 101;
    updateComboBoxes();
    updateGeneralInfo();
}

void MainWindow::on_actionCreateRoute_triggered() {
    QMessageBox::information(this, "Crear Ruta", 
                           "Haga clic en dos estaciones en el mapa para crear una ruta.");
}

void MainWindow::on_actionDeleteRoute_triggered() {
    QMessageBox::information(this, "Eliminar Ruta", 
                           "Haga clic derecho en una línea de ruta para eliminarla.");
}

void MainWindow::on_actionCreateStation_triggered() {
    QMessageBox::information(this, "Crear Estación", 
                           "Haga doble clic en un área vacía para crear una estación.");
}

void MainWindow::on_actionDeleteStation_triggered() {
    QMessageBox::information(this, "Eliminar Estación", 
                           "Haga clic derecho en una estación para eliminarla.");
}

void MainWindow::on_actionExportTraversals_triggered() {
    exportTraversals();
}

void MainWindow::on_actionGenerateReport_triggered() {
    generateReport();
}
