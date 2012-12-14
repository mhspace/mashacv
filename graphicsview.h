#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QTransform>
#include "qgraphicsviewextended.h"
namespace Ui {
class GraphicsView;
}

class GraphicsView : public QWidget
{
    Q_OBJECT
//DONE: реализовать zoom to fit
//DONE: реализовать отображения текущего зума в процентах
public:
    explicit GraphicsView(QWidget *parent = 0);
    ~GraphicsView();

    //from QGraphicsView
    void setSceneRect ( const QRectF & rect );
    void setSceneRect ( qreal x, qreal y, qreal w, qreal h );
    void setTransform ( const QTransform & matrix, bool combine = false );
    void setScene ( QGraphicsScene * scene );
    void fitInView ( const QRectF & rect, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio );
    QRectF 	sceneRect () const;
    QTransform transform () const;
    void setDragMode(QGraphicsView::DragMode mode);
    //endof from QGraphicsView


signals:
    //from QGraphicsViewExtended
    void mousePressed(QPoint point);
    void mouseMoved(QPoint point);
    void mouseMovedWhileLeftMouseButtonPressed(QPoint point);
    void mouseMovedWhileLeftMouseButtonNotPressed(QPoint point);
    void mouseEntered();
    void mouseLeaved();
    //endof from QGraphicsViewExtended
    
protected:
    void changeEvent(QEvent *e);
    
private slots:
    void on_zoomInButton_clicked();
    void on_zoomOutButton_clicked();
    void on_oneToOneZoomButton_clicked();

    //for QGraphicsViewExtended
    void on_graphicsView_mousePressed(QPoint point);
    void on_graphicsView_mouseMoved(QPoint point);
    void on_graphicsView_mouseMovedWhileLeftMouseButtonPressed(QPoint point);
    void on_graphicsView_mouseMovedWhileLeftMouseButtonNotPressed(QPoint point);
    void on_graphicsView_mouseEntered();
    void on_graphicsView_mouseLeaved();
    //endof for QGraphicsViewExtended

    void on_zoomToFitButton_clicked();

private:
    Ui::GraphicsView *ui;
    void updateZoom();
};

#endif // GRAPHICSVIEW_H
