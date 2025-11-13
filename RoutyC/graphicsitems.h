#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPointF>
#include <QTimer>

class MainWindow;

class DraggableStation : public QGraphicsEllipseItem {
public:
    DraggableStation(int stationId, double x, double y, double diameter, MainWindow* window);
    
    int getStationId() const;
    QGraphicsTextItem* getLabel();
    void setLabel(QGraphicsTextItem* textItem);
    void setMainWindow(MainWindow* window);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    
private:
    int stationId;
    QGraphicsTextItem* label;
    MainWindow* mainWindow;
    bool wasDragged;
    QPointF pressPos;
    QTimer* scaleTimer;
    qreal currentScale;
    qreal targetScale;
    
    void updateScale();
};

class ClickableRoute : public QGraphicsLineItem {
public:
    ClickableRoute(int id1, int id2, const QLineF& line, MainWindow* window);
    
    int getSourceId() const;
    int getDestId() const;
    void setMainWindow(MainWindow* window);
    void setBlocked(bool blocked);
    bool isBlocked() const;
    void updateVisualState();
    QPainterPath shape() const override;
    
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    
private:
    int sourceId;
    int destId;
    MainWindow* mainWindow;
    bool blocked;
};

#endif
