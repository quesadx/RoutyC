#include "MainWIndow.h"
#include "ui_MainWindow.h"
#include "GraphicsItems.h"
#include "FIleManager.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <fstream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent), ui(new Ui::MainWindow), 
      scene(nullptr), networkManager(nullptr),
      nextStationId(101), selectedStationId(-1),
      currentAnimationStep(0), isAnimating(false), currentFilePath("") {
    ui->setupUi(this);
    statusBar()->hide();
    
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &MainWindow::animateNextStep);
    
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
    scene->setSceneRect(-2000, -2000, 4800, 4600);
    
    // Parte que dibuja la "ciudad" a puro qt
    QPixmap cityPattern(240, 240);
    cityPattern.fill(QColor(26, 26, 26));
    QPainter painter(&cityPattern);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(25, 35, 48)));
    
    painter.setPen(QPen(QColor(50, 50, 52), 8));
    painter.drawLine(0, 80, 240, 80);
    
    painter.setPen(QPen(QColor(42, 42, 44), 4));
    painter.drawLine(0, 150, 240, 150);
    painter.drawLine(70, 0, 70, 240);
    painter.drawLine(190, 0, 190, 240);
    
    painter.setPen(QPen(QColor(36, 36, 38), 2));
    painter.drawLine(0, 30, 130, 30);
    painter.drawLine(0, 116, 240, 116);
    painter.drawLine(0, 190, 240, 190);
    painter.drawLine(30, 0, 30, 240);
    painter.drawLine(120, 90, 120, 240);
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(34, 34, 36)));
    painter.drawRect(8, 6, 18, 18);
    painter.drawRect(36, 6, 26, 18);
    painter.drawRect(170, 6, 16, 24);
    painter.drawRect(8, 90, 28, 20);
    painter.drawRect(80, 90, 32, 20);
    
    painter.setBrush(QBrush(QColor(31, 31, 33)));
    painter.drawRect(8, 160, 18, 24);
    painter.drawRect(36, 124, 12, 20);
    painter.drawRect(200, 96, 20, 16);
    painter.drawRect(200, 160, 16, 24);
    
    painter.setBrush(QBrush(QColor(29, 29, 31)));
    painter.drawRect(80, 40, 10, 10);
    painter.drawRect(100, 40, 8, 8);
    painter.drawRect(80, 200, 10, 12);
    painter.drawRect(130, 124, 8, 10);
    painter.drawRect(150, 200, 10, 10);
    
    scene->setBackgroundBrush(QBrush(cityPattern));
    ui->gvArea->setScene(scene);
    
    networkManager = new NetworkManager(scene, this);
    
    ui->gvArea->viewport()->installEventFilter(this);
    ui->gvArea->setRenderHint(QPainter::Antialiasing);
    ui->gvArea->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->gvArea->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->gvArea->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->gvArea->setCacheMode(QGraphicsView::CacheBackground);
}

void MainWindow::setupAlgorithms() {
    algorithms.push_back(new DijkstraAlgorithm());
    algorithms.push_back(new BFSAlgorithm());
    algorithms.push_back(new DFSAlgorithm());
    algorithms.push_back(new FloydWarshallAlgorithm());
    algorithms.push_back(new PrimAlgorithm());
    algorithms.push_back(new KruskalAlgorithm());
    
    for (PathAlgorithm* algo : algorithms) {
        ui->cbAlgorithm->addItem(QString::fromStdString(algo->getName()));
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
            zoomIn();
            event->accept();
            return;
        } else if (event->key() == Qt::Key_Minus) {
            zoomOut();
            event->accept();
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
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
    vector<StationNode*> stations = networkManager->getAllStations();
    for (StationNode* station : stations) {
        if (QString::fromStdString(station->name) == name) {
            QMessageBox::warning(this, "Error", 
                               "Ya existe una estación con ese nombre.");
            return;
        }
    }
    
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
    
    vector<StationNode*> stations = networkManager->getAllStations();
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
    vector<StationNode*> stations = networkManager->getAllStations();
    int totalStations = stations.size();
    
    TransportGraph* graph = networkManager->getGraph();
    int totalRoutes = 0;
    vector<int> allStations = graph->getAllStations();
    for (int stationId : allStations) {
        totalRoutes += graph->getAllConnectedStations(stationId).size();
    }
    totalRoutes /= 2;
    
    QString info = QString("Estaciones totales: %1 | Rutas totales: %2")
                   .arg(totalStations)
                   .arg(totalRoutes);
    ui->lblGeneralInfo->setText(info);
}

void MainWindow::zoomIn() {
    ui->gvArea->scale(1.15, 1.15);
}

void MainWindow::zoomOut() {
    ui->gvArea->scale(1.0 / 1.15, 1.0 / 1.15);
}

void MainWindow::handleStationClick(DraggableStation* station) {
    int clickedId = station->getStationId();
    
    if (selectedStationId == -1) {
        selectedStationId = clickedId;
        station->setBrush(QBrush(QColor(147, 51, 234)));
    } else if (selectedStationId == clickedId) {
        selectedStationId = -1;
        station->setBrush(QBrush(QColor(124, 58, 237)));
    } else {
        int firstId = selectedStationId;
        int secondId = clickedId;
        
        DraggableStation* firstStation = networkManager->getStation(firstId);
        if (firstStation) {
            firstStation->setBrush(QBrush(QColor(124, 58, 237)));
        }
        
        if (networkManager->getGraph()->hasRoute(firstId, secondId)) {
            QMessageBox::information(this, "Ruta Existente", 
                                   "Ya existe una ruta entre estas estaciones.");
            selectedStationId = -1;
            return;
        }
        
        bool ok;
        int travelTime = QInputDialog::getInt(this, "Crear Ruta", 
                                              "Ingrese el tiempo de viaje (minutos):", 
                                              10, 1, 1000, 1, &ok);
        if (ok) {
            networkManager->createRoute(firstId, secondId, travelTime);
            
            updateGeneralInfo();
            
            if (!currentFilePath.isEmpty()) {
                autoSaveData();
            }
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

void MainWindow::handleStationRename(int stationId) {
    StationNode* node = networkManager->getTree()->searchStation(stationId);
    if (!node) {
        return;
    }
    
    QString currentName = QString::fromStdString(node->name);
    bool ok;
    QString newName = QInputDialog::getText(this, "Renombrar Estación",
                                           "Ingrese el nuevo nombre de la estación:",
                                           QLineEdit::Normal, currentName, &ok);
    
    if (ok && !newName.isEmpty() && newName != currentName) {
        vector<StationNode*> stations = networkManager->getAllStations();
        for (StationNode* station : stations) {
            if (station->id != stationId && 
                QString::fromStdString(station->name) == newName) {
                QMessageBox::warning(this, "Error", 
                                   "Ya existe una estación con ese nombre.");
                return;
            }
        }
        
        node->name = newName.toStdString();
        
        DraggableStation* stationItem = networkManager->getStation(stationId);
        if (stationItem && stationItem->getLabel()) {
            stationItem->getLabel()->setPlainText(newName);
            
            QRectF bounds = stationItem->rect();
            QPointF center = bounds.center() + stationItem->pos();
            stationItem->getLabel()->setPos(
                center.x() - stationItem->getLabel()->boundingRect().width() / 2,
                center.y() + bounds.height() / 2
            );
        }
        
        updateComboBoxes();
    }
}

void MainWindow::handleRouteDelete(int sourceId, int destId) {
    networkManager->deleteRoute(sourceId, destId);
    updateGeneralInfo();
}

void MainWindow::handleRouteToggleClosure(int sourceId, int destId) {
    TransportGraph* graph = networkManager->getGraph();
    
    if (graph->isRouteBlocked(sourceId, destId)) {
        graph->unblockRoute(sourceId, destId);
    } else {
        graph->blockRoute(sourceId, destId);
    }
    
    networkManager->updateRouteVisualState(sourceId, destId);
    
    if (!currentFilePath.isEmpty()) {
        autoSaveData();
    }
}

void MainWindow::handleRouteModifyWeight(int sourceId, int destId) {
    TransportGraph* graph = networkManager->getGraph();
    int currentWeight = graph->getRouteTime(sourceId, destId);
    
    bool ok;
    int newWeight = QInputDialog::getInt(this, "Modificar Ruta", 
                                         "Ingrese el nuevo tiempo de viaje (minutos):", 
                                         currentWeight, 1, 1000, 1, &ok);
    if (ok && newWeight != currentWeight) {
        networkManager->createRoute(sourceId, destId, newWeight);
        
        if (!currentFilePath.isEmpty()) {
            autoSaveData();
        }
    }
}

void MainWindow::updateRoutePositionsDuringDrag(int stationId, const QPointF& center) {
    networkManager->updateRoutePosition(stationId, center);
}

void MainWindow::exportTraversals() {
    QString filename = "recorridos_rutas.txt";
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo para escritura");
        return;
    }
    
    QTextStream out(&file);
    
    out << "~ Recorridos del Árbol de Estaciones ~\n\n";
    
    out << "Recorrido In-Order:\n";
    vector<StationNode*> inOrder = networkManager->getAllStations();
    for (StationNode* node : inOrder) {
        out << "  ID: " << node->id << " - " << QString::fromStdString(node->name) << "\n";
    }
    out << "\n";
    
    out << "Recorrido Pre-Order:\n";
    vector<StationNode*> preOrder = networkManager->getTree()->getPreOrderTraversal();
    for (StationNode* node : preOrder) {
        out << "  ID: " << node->id << " - " << QString::fromStdString(node->name) << "\n";
    }
    out << "\n";
    
    out << "Recorrido Post-Order:\n";
    vector<StationNode*> postOrder = networkManager->getTree()->getPostOrderTraversal();
    for (StationNode* node : postOrder) {
        out << "  ID: " << node->id << " - " << QString::fromStdString(node->name) << "\n";
    }
    out << "\n";
    
    out << "~ Grafo de transporte ~\n\n";
    
    TransportGraph* graph = networkManager->getGraph();
    vector<int> allStations = graph->getAllStations();
    
    for (int stationId : allStations) {
        StationNode* node = networkManager->getTree()->searchStation(stationId);
        if (node) {
            out << "Estación " << node->id << " (" << QString::fromStdString(node->name) << "):\n";
            vector<int> neighbors = graph->getConnectedStations(stationId);
            for (int neighbor : neighbors) {
                StationNode* neighborNode = networkManager->getTree()->searchStation(neighbor);
                if (neighborNode) {
                    int weight = graph->getRouteTime(stationId, neighbor);
                    out << "  -> " << neighborNode->id << " (" 
                        << QString::fromStdString(neighborNode->name) << ") - " << weight << " min\n";
                }
            }
            out << "\n";
        }
    }
    
    file.close();
    QMessageBox::information(this, "Éxito", "Recorridos exportados exitosamente a recorridos_rutas.txt");
}

void MainWindow::generateReport() {
    QString filename = "reportes.txt";
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo para escritura");
        return;
    }
    
    QTextStream out(&file);
    
    out << "~ Reporte de la red de transporte de La Mancha ~\n";
    
    vector<StationNode*> stations = networkManager->getAllStations();
    out << "Total de Estaciones: " << stations.size() << "\n\n";
    
    TransportGraph* graph = networkManager->getGraph();
    int totalRoutes = 0;
    vector<int> allStations = graph->getAllStations();
    for (int stationId : allStations) {
        totalRoutes += graph->getConnectedStations(stationId).size();
    }
    totalRoutes /= 2;
    out << "Total de Rutas: " << totalRoutes << "\n\n";
    
    out << "~ Estaciones ~\n";
    for (StationNode* node : stations) {
        out << "ID: " << node->id << "\n";
        out << "Nombre: " << QString::fromStdString(node->name) << "\n";
        out << "Posición: (" << node->x << ", " << node->y << ")\n";
        
        vector<int> neighbors = graph->getConnectedStations(node->id);
        out << "Conexiones: " << neighbors.size() << "\n";
        
        for (int neighbor : neighbors) {
            StationNode* neighborNode = networkManager->getTree()->searchStation(neighbor);
            if (neighborNode) {
                int weight = graph->getRouteTime(node->id, neighbor);
                out << "  -> " << QString::fromStdString(neighborNode->name) 
                    << " (" << weight << " min)\n";
            }
        }
        out << "\n";
    }

    out << "~ Análisis de conectividad ~\n";
    for (StationNode* node : stations) {
        int connections = graph->getConnectedStations(node->id).size();
        out << QString::fromStdString(node->name) << ": " << connections << " conexión";
        if (connections != 1) out << "es";
        out << "\n";
    }
    
    file.close();
    QMessageBox::information(this, "Éxito", "Reporte generado exitosamente en reportes.txt");
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
    
    if (!networkManager->getGraph()->canReach(originId, destId)) {
        QMessageBox::warning(this, "Grafo Desconectado", 
                           "No existe conexión entre el origen y el destino. Por favor agregue rutas para conectar estas estaciones.");
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
        ui->pteOutput->appendPlainText("¡Ruta encontrada!");
        ui->pteOutput->appendPlainText("Costo total: " + QString::number(result.totalCost) + " minutos");
        ui->pteOutput->appendPlainText("");
        ui->pteOutput->appendPlainText("Visualización en progreso...");
        
        startAnimation(result);
    } else {
        ui->pteOutput->appendPlainText("¡No se encontró una ruta!");
        ui->pteOutput->appendPlainText("");
        ui->pteOutput->appendPlainText("Pasos de ejecución:");
        
        for (const string& step : result.steps) {
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
                                                    "Archivos de texto (*.txt)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    if (!filename.endsWith(".txt", Qt::CaseInsensitive)) {
        filename += ".txt";
    }
    
    if (FileManager::saveToFile(filename.toStdString(), networkManager->getTree(), 
                                networkManager->getGraph())) {
        currentFilePath = filename;
        QMessageBox::information(this, "Éxito", "Red guardada exitosamente");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo guardar la red");
    }
}

void MainWindow::on_actionLoad_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, "Cargar Red", "", 
                                                    "Archivos de texto (*.txt)");
    
    if (filename.isEmpty()) {
        return;
    }
    
    networkManager->clearAll();
    selectedStationId = -1;
    
    if (FileManager::loadFromFile(filename.toStdString(), networkManager->getTree(), 
                                  networkManager->getGraph())) {
        currentFilePath = filename;
        networkManager->reconstructFromData(networkManager->getTree(), networkManager->getGraph());
        networkManager->updateAllRouteVisualStates();
        
        updateComboBoxes();
        
        vector<StationNode*> stations = networkManager->getAllStations();
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
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Crear Ruta");
    msgBox.setText("Haga clic en dos estaciones en el mapa para crear una ruta.");
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();
}

void MainWindow::on_actionDeleteRoute_triggered() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Eliminar Ruta");
    msgBox.setText("Haga clic derecho en una línea de ruta para eliminarla.");
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();
}

void MainWindow::on_actionModifyRoute_triggered() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Modificar Ruta");
    msgBox.setText("Haga clic derecho en una línea de ruta para modificar su tiempo de viaje, bloquearla o eliminarla.");
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();
}

void MainWindow::on_actionCreateStation_triggered() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Crear Estación");
    msgBox.setText("Haga doble clic en un área vacía del mapa para crear una estación.");
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();
}

void MainWindow::autoLoadData() {
    ifstream estacionesFile("estaciones.txt");
    if (estacionesFile.good()) {
        estacionesFile.close();
        if (FileManager::loadFromFile("estaciones.txt", networkManager->getTree(), 
                                     networkManager->getGraph())) {
            networkManager->reconstructFromData(networkManager->getTree(), 
                                               networkManager->getGraph());
            updateComboBoxes();
            updateGeneralInfo();
        }
    }
    
    ifstream cierresFile("cierres.txt");
    if (cierresFile.good()) {
        cierresFile.close();
        FileManager::loadClosures("cierres.txt", networkManager->getGraph());
        networkManager->updateAllRouteVisualStates();
    }
}

void MainWindow::autoSaveData() {
    if (currentFilePath.isEmpty()) {
        return;
    }
    
    FileManager::saveToFile(currentFilePath.toStdString(), networkManager->getTree(), 
                           networkManager->getGraph());
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionDeleteStation_triggered() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Eliminar Estación");
    msgBox.setText("Haga clic derecho en una estación para eliminarla.");
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();
}

void MainWindow::on_actionSearchStation_triggered() {
    bool ok;
    QString searchText = QInputDialog::getText(this, "Buscar Estación",
                                              "Ingrese el nombre de la estación a buscar:",
                                              QLineEdit::Normal, "", &ok);
    
    if (ok && !searchText.isEmpty()) {
        vector<StationNode*> stations = networkManager->getAllStations();
        StationNode* found = nullptr;
        
        for (StationNode* station : stations) {
            if (QString::fromStdString(station->name).contains(searchText, Qt::CaseInsensitive)) {
                found = station;
                break;
            }
        }
        
        if (found) {
            DraggableStation* stationItem = networkManager->getStation(found->id);
            if (stationItem) {
                networkManager->clearHighlights();
                stationItem->setBrush(QBrush(QColor(250, 204, 21)));
                
                QRectF bounds = stationItem->rect();
                QPointF center = bounds.center() + stationItem->pos();
                ui->gvArea->centerOn(center);
                
                QMessageBox msgBox(this);
                msgBox.setWindowTitle("Estación Encontrada");
                msgBox.setText(QString("Se encontró la estación: %1").arg(QString::fromStdString(found->name)));
                msgBox.setIcon(QMessageBox::NoIcon);
                msgBox.exec();
            }
        } else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("No Encontrada");
            msgBox.setText(QString("No se encontró ninguna estación con el nombre: %1").arg(searchText));
            msgBox.setIcon(QMessageBox::NoIcon);
            msgBox.exec();
        }
    }
}

void MainWindow::on_actionExportTraversals_triggered() {
    exportTraversals();
}

void MainWindow::on_actionGenerateReport_triggered() {
    generateReport();
}

void MainWindow::startAnimation(const PathResult& result) {
    if (isAnimating) {
        return;
    }
    
    currentResult = result;
    currentAnimationStep = 0;
    isAnimating = true;
    networkManager->clearHighlights();
    
    animationTimer->start(500);
}

void MainWindow::animateNextStep() {
    if (currentAnimationStep >= (int)currentResult.visualSteps.size()) {
        finishAnimation();
        return;
    }
    
    visualizeStep(currentResult.visualSteps[currentAnimationStep]);
    currentAnimationStep++;
}

void MainWindow::visualizeStep(const VisualizationStep& step) {
    networkManager->clearHighlights();
    
    for (int nodeId : step.visitedNodes) {
        auto it = networkManager->getStationItems().find(nodeId);
        if (it != networkManager->getStationItems().end()) {
            it->second->setBrush(QBrush(QColor(34, 211, 238)));
        }
    }
    
    for (const auto& edge : step.visitedEdges) {
        pair<int, int> key = edge.first < edge.second ? 
            make_pair(edge.first, edge.second) : 
            make_pair(edge.second, edge.first);
        
        auto it = networkManager->getRouteItems().find(key);
        if (it != networkManager->getRouteItems().end()) {
            it->second->setPen(QPen(QColor(34, 211, 238), 4));
        }
    }
    
    ui->pteOutput->appendPlainText(QString::fromStdString(step.description));
}

void MainWindow::finishAnimation() {
    animationTimer->stop();
    isAnimating = false;
    
    networkManager->highlightPath(currentResult.path);
    
    ui->pteOutput->appendPlainText("");
    ui->pteOutput->appendPlainText("Ruta final:");
    
    QString pathStr = "";
    for (size_t i = 0; i < currentResult.path.size(); i++) {
        StationNode* node = networkManager->getTree()->searchStation(currentResult.path[i]);
        if (node) {
            pathStr += QString::fromStdString(node->name);
            if (i < currentResult.path.size() - 1) {
                pathStr += " > ";
            }
        }
    }
    ui->pteOutput->appendPlainText(pathStr);
}
