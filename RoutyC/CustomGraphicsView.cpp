#include "CustomGraphicsView.h"
#include <QPainter>
#include <QRadialGradient>
#include <cmath>

CustomGraphicsView::CustomGraphicsView(QWidget* parent)
    : QGraphicsView(parent), isMouseInside(false) {
    
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &CustomGraphicsView::updateGlowAnimation);
    animationTimer->start(16);
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event) {
    targetMousePos = event->pos();
    if (!isMouseInside) {
        currentGlowPos = targetMousePos;
        isMouseInside = true;
    }
    viewport()->update();
    QGraphicsView::mouseMoveEvent(event);
}

void CustomGraphicsView::leaveEvent(QEvent* event) {
    isMouseInside = false;
    viewport()->update();
    QGraphicsView::leaveEvent(event);
}

void CustomGraphicsView::enterEvent(QEnterEvent* event) {
    isMouseInside = true;
    QGraphicsView::enterEvent(event);
}

void CustomGraphicsView::updateGlowAnimation() {
    if (isMouseInside) {
        smoothGlowPosition();
        viewport()->update();
    }
}

void CustomGraphicsView::smoothGlowPosition() {
    double smoothFactor = 0.12;
    
    double dx = targetMousePos.x() - currentGlowPos.x();
    double dy = targetMousePos.y() - currentGlowPos.y();
    
    currentGlowPos.setX(currentGlowPos.x() + dx * smoothFactor);
    currentGlowPos.setY(currentGlowPos.y() + dy * smoothFactor);
}

void CustomGraphicsView::paintEvent(QPaintEvent* event) {
    QGraphicsView::paintEvent(event);
    
    if (isMouseInside) {
        QPainter painter(viewport());
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setCompositionMode(QPainter::CompositionMode_Plus);
        
        QRadialGradient gradient(currentGlowPos, 200);
        gradient.setColorAt(0.0, QColor(124, 58, 237, 28));
        gradient.setColorAt(0.15, QColor(124, 58, 237, 24));
        gradient.setColorAt(0.3, QColor(124, 58, 237, 18));
        gradient.setColorAt(0.5, QColor(124, 58, 237, 12));
        gradient.setColorAt(0.7, QColor(124, 58, 237, 6));
        gradient.setColorAt(0.85, QColor(124, 58, 237, 3));
        gradient.setColorAt(1.0, QColor(124, 58, 237, 0));
        
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(currentGlowPos, 200, 200);
    }
}
