#include "layerdockwidget.h"
#include "ui_layerdockwidget.h"
#include <QDebug>
#include <QGraphicsItem>

LayerDockWidget::LayerDockWidget(QGraphicsScene *scene, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LayerDockWidget)
{
    this->layers = new QVector<QGraphicsItem*>(5);
    this->isLayerExsists = new QVector<bool>(5, false);
    ui->setupUi(this);
    this->scene = scene;
    this->listModel = new LayerListModel();
    ui->layerListView->setModel(this->listModel);
}

LayerDockWidget::~LayerDockWidget()
{
    delete ui;
}

void LayerDockWidget::updateLayer(int n, QGraphicsItem *layer)
{

}

void LayerDockWidget::updateLayer(int n, QImage *image)
{
    if (this->isLayerExsists->at(n))
    {
        this->scene->removeItem(this->layers->at(n));
        this->layers->at(n)->~QGraphicsItem();
    }
    this->layers->replace(n, this->scene->addPixmap(QPixmap::fromImage(*image)));
    this->layers->at(n)->setZValue(n);
    ((QGraphicsPixmapItem*)(this->layers->at(n)))->setAcceptedMouseButtons(Qt::NoButton);
    ((QGraphicsPixmapItem*)(this->layers->at(n)))->setFlags(0);
    this->isLayerExsists->replace(n, true);
}

void LayerDockWidget::updateBaseImage(QImage *image)
{
    this->updateLayer(0, image);
}

void LayerDockWidget::updateColorMask(QImage *image)
{
    this->updateLayer(1, image);
}

void LayerDockWidget::updateImageMask(QImage *image)
{
    this->updateLayer(3, image);
}

void LayerDockWidget::updateDebugLayer(QImage *image)
{
    this->updateLayer(2, image);
}

void LayerDockWidget::updateDisabletItemsOverlayLayer(QImage *image)
{
    this->updateLayer(4, image);
}

void LayerDockWidget::updateBrushLayerPosition(QPoint point)
{
    int radius = (int)(((QGraphicsPixmapItem*)(this->layers->at(10)))->boundingRect().height()/2);
    ((QGraphicsPixmapItem*)(this->layers->at(10)))->setPos(point.x() - radius, point.y() - radius);
}

void LayerDockWidget::updateBrush(QPoint point, QImage *image)
{
    this->updateLayer(10, image);
}

void LayerDockWidget::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
