#include "graphicsitems.h"
#include "mainwindow.h"
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QBrush>
#include <QPen>
#include <QColor>

DraggableStation::DraggableStation(int stationId, double x, double y, double diameter, MainWindow* window)
    : QGraphicsEllipseItem(x - diameter/2, y - diameter/2, diameter, diameter),
      stationId(stationId), label(nullptr), mainWindow(window), wasDragged(false) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setBrush(QBrush(QColor(124, 58, 237)));
    setPen(QPen(QColor(167, 139, 250), 2));
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
    QAction* deleteAction = menu.addAction("Eliminar Estación");
    
    QAction* selected = menu.exec(event->screenPos());
    if (selected == deleteAction && mainWindow) {
        mainWindow->handleStationDelete(stationId);
    }
}

ClickableRoute::ClickableRoute(int id1, int id2, const QLineF& line, MainWindow* window)
    : QGraphicsLineItem(line), sourceId(id1), destId(id2), mainWindow(window) {
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

void ClickableRoute::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QMenu menu;
    QAction* deleteAction = menu.addAction("Eliminar Ruta");
    
    QAction* selected = menu.exec(event->screenPos());
    if (selected == deleteAction && mainWindow) {
        mainWindow->handleRouteDelete(sourceId, destId);
    }
}
