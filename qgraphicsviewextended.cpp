#include "qgraphicsviewextended.h"
#include <QMouseEvent>


QGraphicsViewExtended::QGraphicsViewExtended(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setMouseTracking(true);
    this->isMousePressed = false;

}


void QGraphicsViewExtended::mousePressEvent(QMouseEvent *event)
{
    this->isMousePressed = true;
    QPointF pointF = (this->mapToScene(event->pos()));
    emit mousePressed(QPoint(pointF.x(), pointF.y()));
    this->QGraphicsView::mousePressEvent(event);
}

void QGraphicsViewExtended::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pointF = (this->mapToScene(event->pos()));
    QPoint point(pointF.x(), pointF.y());
    emit mouseMoved(point);
    if (this->isMousePressed)
        emit mouseMovedWhileLeftMouseButtonPressed(point);
    else
        emit mouseMovedWhileLeftMouseButtonNotPressed(point);

    this->QGraphicsView::mouseMoveEvent(event);
}

void QGraphicsViewExtended::mouseReleaseEvent(QMouseEvent *event)
{
    this->isMousePressed = false;
    this->QGraphicsView::mouseReleaseEvent(event);
}

void QGraphicsViewExtended::enterEvent(QEvent *event)
{
    emit mouseEntered();
}

void QGraphicsViewExtended::leaveEvent(QEvent *event)
{
    emit mouseLeaved();
}

void QGraphicsViewExtended::dragEnterEvent(QDragEnterEvent *event)
{
    //TODO: проверить, надо ли оно.
}


