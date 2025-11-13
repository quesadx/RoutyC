#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QTimer>
#include <QPointF>

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit CustomGraphicsView(QWidget* parent = nullptr);
    
protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void updateGlowAnimation();

private:
    QPointF targetMousePos;
    QPointF currentGlowPos;
    bool isMouseInside;
    QTimer* animationTimer;
    
    void smoothGlowPosition();
};

#endif
