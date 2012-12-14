#ifndef LAYERDOCKWIDGET_H
#define LAYERDOCKWIDGET_H

#include <QDockWidget>
#include <QAbstractListModel>
#include <QGraphicsScene>
#include <QVector>
#include "layerlistmodel.h"

namespace Ui {
class LayerDockWidget;
}

class LayerDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit LayerDockWidget(QGraphicsScene *scene, QWidget *parent = 0);
    ~LayerDockWidget();
    void updateLayer(int n, QGraphicsItem *layer);
    void updateLayer(int n, QImage *image);
    void updateBaseImage(QImage *image);
    void updateColorMask(QImage *image);
    void updateImageMask(QImage *image);
    void updateDebugLayer(QImage *image);
    void updateDisabletItemsOverlayLayer(QImage *image);
    void updateBrushLayerPosition(QPoint point);
    void updateBrush(QPoint point, QGraphicsPixmapItem *pixmapItem);
    void updateBrushVisibility(bool visible);
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::LayerDockWidget *ui;
    QGraphicsScene *scene;
    LayerListModel *listModel;
    QVector<QGraphicsItem*> *layers;
    QVector<bool> *isLayerExsists;
};

#endif // LAYERDOCKWIDGET_H
