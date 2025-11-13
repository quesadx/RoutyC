#include "GraphicsItems.h"
#include "MainWIndow.h"
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QBrush>
#include <QPen>
#include <QColor>

DraggableStation::DraggableStation(int stationId, double x, double y, double diameter, MainWindow* window)
    : QGraphicsEllipseItem(x - diameter/2, y - diameter/2, diameter, diameter),
      stationId(stationId), label(nullptr), mainWindow(window), wasDragged(false),
      currentScale(1.0), targetScale(1.0) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setBrush(QBrush(QColor(124, 58, 237)));
    setPen(QPen(QColor(167, 139, 250), 2));
    setTransformOriginPoint(rect().center());
    
    scaleTimer = new QTimer();
    scaleTimer->setInterval(16);
    QObject::connect(scaleTimer, &QTimer::timeout, [this]() { updateScale(); });
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

void DraggableStation::setMainWindow(MainWindow* window) {
    mainWindow = window;
}

void DraggableStation::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    wasDragged = false;
    pressPos = event->scenePos();
    QGraphicsEllipseItem::mousePressEvent(event);
}

void DraggableStation::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    QPointF currentPos = event->scenePos();
    double distance = (currentPos - pressPos).manhattanLength();
    
    if (distance > 5.0) {
        wasDragged = true;
    }
    
    QGraphicsEllipseItem::mouseMoveEvent(event);
    
    if (label) {
        QRectF bounds = rect();
        label->setPos(bounds.center().x() + pos().x() - label->boundingRect().width() / 2,
                      bounds.center().y() + pos().y() + bounds.height() / 2);
    }
    
    if (mainWindow) {
        QRectF bounds = rect();
        QPointF center = bounds.center() + pos();
        mainWindow->updateRoutePositionsDuringDrag(stationId, center);
    }
}

void DraggableStation::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsEllipseItem::mouseReleaseEvent(event);
    
    if (!wasDragged && mainWindow) {
        mainWindow->handleStationClick(this);
    } else if (wasDragged && mainWindow) {
        QRectF bounds = rect();
        QPointF center = bounds.center() + pos();
        mainWindow->handleStationMoved(stationId, center);
    }
    
    wasDragged = false;
}

void DraggableStation::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QMenu menu;
    QAction* renameAction = menu.addAction("Renombrar Estación");
    QAction* deleteAction = menu.addAction("Eliminar Estación");
    
    QAction* selected = menu.exec(event->screenPos());
    if (selected == renameAction && mainWindow) {
        mainWindow->handleStationRename(stationId);
    } else if (selected == deleteAction && mainWindow) {
        mainWindow->handleStationDelete(stationId);
    }
}

ClickableRoute::ClickableRoute(int id1, int id2, const QLineF& line, MainWindow* window)
    : QGraphicsLineItem(line), sourceId(id1), destId(id2), mainWindow(window), blocked(false) {
    setFlag(QGraphicsItem::ItemIsSelectable);
    setPen(QPen(QColor(107, 114, 128), 2));
}

int ClickableRoute::getSourceId() const {
    return sourceId;
}

int ClickableRoute::getDestId() const {
    return destId;
}

void ClickableRoute::setMainWindow(MainWindow* window) {
    mainWindow = window;
}

void ClickableRoute::setBlocked(bool isBlocked) {
    blocked = isBlocked;
    updateVisualState();
}

bool ClickableRoute::isBlocked() const {
    return blocked;
}

void ClickableRoute::updateVisualState() {
    if (blocked) {
        QPen blockedPen(QColor(239, 68, 68), 3);
        blockedPen.setStyle(Qt::DashLine);
        setPen(blockedPen);
    } else {
        setPen(QPen(QColor(107, 114, 128), 2));
    }
}

QPainterPath ClickableRoute::shape() const {
    QPainterPath path;
    QPainterPathStroker stroker;
    stroker.setWidth(15);
    path.moveTo(line().p1());
    path.lineTo(line().p2());
    return stroker.createStroke(path);
}

void ClickableRoute::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QMenu menu;
    QAction* modifyWeightAction = menu.addAction("Modificar Tiempo de Viaje");
    QAction* toggleClosureAction = menu.addAction(blocked ? "Desbloquear Ruta" : "Bloquear Ruta (Cierre)");
    QAction* deleteAction = menu.addAction("Eliminar Ruta");
    
    QAction* selected = menu.exec(event->screenPos());
    if (selected == modifyWeightAction && mainWindow) {
        mainWindow->handleRouteModifyWeight(sourceId, destId);
    } else if (selected == toggleClosureAction && mainWindow) {
        mainWindow->handleRouteToggleClosure(sourceId, destId);
    } else if (selected == deleteAction && mainWindow) {
        mainWindow->handleRouteDelete(sourceId, destId);
    }
}

void DraggableStation::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    targetScale = 1.15;
    if (!scaleTimer->isActive()) {
        scaleTimer->start();
    }
    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void DraggableStation::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    targetScale = 1.0;
    if (!scaleTimer->isActive()) {
        scaleTimer->start();
    }
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void DraggableStation::updateScale() {
    double difference = targetScale - currentScale;
    
    if (qAbs(difference) < 0.001) {
        currentScale = targetScale;
        setScale(currentScale);
        scaleTimer->stop();
    } else {
        currentScale += difference * 0.2;
        setScale(currentScale);
    }
}
