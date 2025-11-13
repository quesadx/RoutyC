#include "customgraphicsview.h"
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
    double smoothFactor = 0.15;
    
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
        painter.setCompositionMode(QPainter::CompositionMode_Plus);
        
        QRadialGradient gradient(currentGlowPos, 150);
        gradient.setColorAt(0.0, QColor(124, 58, 237, 80));
        gradient.setColorAt(0.3, QColor(124, 58, 237, 40));
        gradient.setColorAt(0.6, QColor(124, 58, 237, 15));
        gradient.setColorAt(1.0, QColor(124, 58, 237, 0));
        
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(currentGlowPos, 150, 150);
        
        QRadialGradient innerGlow(currentGlowPos, 80);
        innerGlow.setColorAt(0.0, QColor(168, 85, 247, 60));
        innerGlow.setColorAt(0.5, QColor(147, 51, 234, 30));
        innerGlow.setColorAt(1.0, QColor(124, 58, 237, 0));
        
        painter.setBrush(innerGlow);
        painter.drawEllipse(currentGlowPos, 80, 80);
    }
}
