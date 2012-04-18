#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QGraphicsScene *scene = 0);
    
signals:
    void mousePressed(QPointF point);
    void mouseMoved(QPointF point);
public slots:

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    
};

#endif // GRAPHICSVIEW_H
