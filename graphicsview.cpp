#include "graphicsview.h"
#include "ui_graphicsview.h"

#define zoomFactor 1.2

GraphicsView::GraphicsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphicsView)
{
    ui->setupUi(this);
    this->updateZoom();
}

GraphicsView::~GraphicsView()
{
    delete ui;
}

void GraphicsView::setSceneRect(const QRectF &rect)
{
    this->ui->graphicsView->setSceneRect(rect);
}

void GraphicsView::setSceneRect(qreal x, qreal y, qreal w, qreal h)
{
    this->ui->graphicsView->setSceneRect(x, y, w, h);
}

void GraphicsView::setTransform(const QTransform &matrix, bool combine)
{
    this->ui->graphicsView->setTransform(matrix, combine);
}

void GraphicsView::setScene(QGraphicsScene *scene)
{
    this->ui->graphicsView->setScene(scene);
}

void GraphicsView::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode)
{
    this->ui->graphicsView->fitInView(rect, aspectRatioMode);
}

QRectF GraphicsView::sceneRect() const
{
    return this->ui->graphicsView->sceneRect();
}

QTransform GraphicsView::transform() const
{
    return this->ui->graphicsView->transform();
}

void GraphicsView::setDragMode(QGraphicsView::DragMode mode)
{
    this->ui->graphicsView->setDragMode(mode);
}

void GraphicsView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void GraphicsView::on_zoomInButton_clicked()
{
    this->setTransform(QTransform::fromScale(zoomFactor, zoomFactor), true);
    this->updateZoom();
}

void GraphicsView::on_zoomOutButton_clicked()
{
    this->setTransform(QTransform::fromScale(1/zoomFactor, 1/zoomFactor), true);
    this->updateZoom();
}

void GraphicsView::on_oneToOneZoomButton_clicked()
{
    this->setTransform(QTransform::fromScale(1, 1), false);
    this->updateZoom();
}

void GraphicsView::on_graphicsView_mousePressed(QPoint point)
{
    emit mousePressed(point);
}

void GraphicsView::on_graphicsView_mouseMoved(QPoint point)
{
    emit mouseMoved(point);
}

void GraphicsView::on_graphicsView_mouseMovedWhileLeftMouseButtonPressed(QPoint point)
{
    emit mouseMovedWhileLeftMouseButtonPressed(point);
}

void GraphicsView::on_graphicsView_mouseMovedWhileLeftMouseButtonNotPressed(QPoint point)
{
    emit mouseMovedWhileLeftMouseButtonNotPressed(point);
}

void GraphicsView::on_graphicsView_mouseEntered()
{
    emit mouseEntered();
}

void GraphicsView::on_graphicsView_mouseLeaved()
{
    emit mouseLeaved();
}

void GraphicsView::on_zoomToFitButton_clicked()
{
    this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
    qApp->processEvents(); // scrollbar correction
    this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);//
    this->updateZoom();
}
#include <QDebug>
#include <QString>
void GraphicsView::updateZoom()
{
    this->ui->zoomValueLabel->setText(QString::number(100*this->transform().m11(), 'f', 0) + "%");
}
