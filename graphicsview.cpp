#include "graphicsview.h"

GraphicsView::GraphicsView(QGraphicsScene* scene) :
    QGraphicsView(scene)
{
    this->setMouseTracking(false);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
    emit mousePressed((this->mapToScene(event->pos())));
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved((this->mapToScene(event->pos())));
}
