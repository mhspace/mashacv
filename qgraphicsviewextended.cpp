#include "qgraphicsviewextended.h"
#include <QMouseEvent>
QGraphicsViewExtended::QGraphicsViewExtended(QWidget *parent) :
    QGraphicsView(parent)
{
}


void QGraphicsViewExtended::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed((this->mapToScene(event->pos())));
    this->QGraphicsView::mousePressEvent(event);
}

void QGraphicsViewExtended::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved((this->mapToScene(event->pos())));
    this->QGraphicsView::mouseMoveEvent(event);
}

void QGraphicsViewExtended::dragEnterEvent(QDragEnterEvent *event)
{

}


