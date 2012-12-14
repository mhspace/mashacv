#ifndef QGRAPHICSVIEWEXTENDED_H
#define QGRAPHICSVIEWEXTENDED_H

#include <QGraphicsView>

class QGraphicsViewExtended : public QGraphicsView
{
    Q_OBJECT
public:
    explicit QGraphicsViewExtended(QWidget *parent = 0);
    
signals:
    void mousePressed(QPoint point);
    void mouseMoved(QPoint point);
    void mouseMovedWhileLeftMouseButtonPressed(QPoint point);
    void mouseMovedWhileLeftMouseButtonNotPressed(QPoint point);
    void mouseEntered();
    void mouseLeaved();
public slots:
    
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    //Forwarding to parent widget
    void dragEnterEvent(QDragEnterEvent * event);
    //void dropEvent( QDropEvent * event );

private:
    bool isMousePressed;
};

#endif // QGRAPHICSVIEWEXTENDED_H
