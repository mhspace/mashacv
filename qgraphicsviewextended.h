#ifndef QGRAPHICSVIEWEXTENDED_H
#define QGRAPHICSVIEWEXTENDED_H

#include <QGraphicsView>

class QGraphicsViewExtended : public QGraphicsView
{
    Q_OBJECT
public:
    explicit QGraphicsViewExtended(QWidget *parent = 0);
    
signals:
    void mousePressed(QPointF point);
    void mouseMoved(QPointF point);
public slots:
    
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);

    //Forwarding to parent widget
    void dragEnterEvent(QDragEnterEvent * event);
    //void dropEvent( QDropEvent * event );
};

#endif // QGRAPHICSVIEWEXTENDED_H
