#include "imageprocessor.h"
#include <QDebug>
#include <QTime>
#define SQR(s)  ((s) * (s))

ImageProcessor::ImageProcessor(QString fileName, QObject *parent) :
    QObject(parent)
{
    this->image = new QImage(fileName);
    this->imageNumberOfPixels = this->image->width()*this->image->height();
    this->imageMatchedPixels = new uchar[this->imageNumberOfPixels];
    this->imageMaskLayer = new uchar[this->imageNumberOfPixels];
    this->imageDisabledItemsOverlay = new uchar[this->imageNumberOfPixels];
    this->graphicsView = new GraphicsView();
    this->graphicsScene = new QGraphicsScene();
    this->colorDockWidget = new ColorDockWidget();
    this->layerManager = new LayerDockWidget(this->graphicsScene);
    this->resultsDockWidget = new ResultsDockWidget();
    this->toolsDockWidget = new ToolsDockWidget(this->imageNumberOfPixels, (this->image->width() > this->image->height()) ? (this->image->width()) : (this->image->height()));
    this->toolsDockWidget->setObjectName("toolsDockWidget");
    this->toolsDockWidget->QObject::setParent(this);
    this->layerManager->updateBaseImage(this->image);
    this->previewTimer = new QTimer();
    this->calculateTimer = new QTimer();
    this->previewTimer->setSingleShot(true);
    this->calculateTimer->setSingleShot(true);
    this->connect(this->previewTimer, SIGNAL(timeout()), this, SLOT(processPreview()));
    this->connect(this->calculateTimer, SIGNAL(timeout()), this, SLOT(processCalculate()));
    this->graphicsView->setScene(this->graphicsScene);
    this->connect(this->colorDockWidget, SIGNAL(colorOrDeltaChanged()), this, SLOT(processDataChange()));

    QMetaObject::connectSlotsByName(this);
}

ImageProcessor::~ImageProcessor()
{
    delete this->colorDockWidget;
    delete this->graphicsScene;
    //delete this->graphicsView;
    delete this->layerManager;
    delete this->toolsDockWidget;
    delete this->image;
    delete this->previewTimer;
    delete[] this->imageMatchedPixels;
    delete[] this->imageMaskLayer;
    delete[] this->imageDisabledItemsOverlay;
}

QList<QDockWidget *> ImageProcessor::getDockWidgets()
{
    QList<QDockWidget *> docks;
    docks.append(this->colorDockWidget);
    //docks.append(this->layerManager);
    //docks.append(this->toolsDockWidget);
    docks.append(this->resultsDockWidget);
    return docks;
}

void ImageProcessor::pick(QPointF point)
{
    //TODO: предусмотреть вариант клика вне картинки.
    //TODO: предусмотреть клик по пикселю с альфа-каналом. (надо ли?)
    this->colorDockWidget->setColor(this->image->pixel(QPoint((int)(point.x()), (int)(point.y()))));
}

void ImageProcessor::processDataChange()
{
    this->previewTimer->start(0);
    this->calculateTimer->stop();
    //this->disableResults();
}

#include <QDebug>
#include <cmath>
#include <QTime>

#define RED(QRgb) ((uchar*)(&QRgb))[1]
#define GREEN(QRgb) ((uchar*)(&QRgb))[2]
#define BLUE(QRgb) ((uchar*)(&QRgb))[3]

#define CIE76_RGB(x, y) sqrt(SQR(RED(x)-RED(y))+SQR(GREEN(x)-GREEN(y))+SQR(BLUE(x)-BLUE(y)))
void ImageProcessor::processPreview()
{
    int size = 0;
    QRgb baseColor = this->colorDockWidget->rgb();
    double maxDelta = this->colorDockWidget->getDelta();

    QTime starttime = QTime::currentTime();
    QImage *image = this->image;

    const uchar * rawData = image->bits();

    QRgb* source = (QRgb*)rawData;

    for (int i = 0; i < this->imageNumberOfPixels; i++)
    {
        if ((this->imageMaskLayer[i] == 0) && (CIE76_RGB(source[i], baseColor) < maxDelta)) //TODO: Проверить корректность значений из макроса
        {
            imageMatchedPixels[i] = (uchar)1;
            size++;
        }
        else
            imageMatchedPixels[i] = (uchar)0;
    }

    QImage *qImageMatchedPixels = new QImage(imageMatchedPixels, image->width(), image->height(), image->width(), QImage::Format_Indexed8);
    qImageMatchedPixels->setColor(0, (uint)0x00000000);
    qImageMatchedPixels->setColor(1, (uint)0xFF00FFFF);

    this->layerManager->updateColorMask(qImageMatchedPixels);
    delete qImageMatchedPixels;

    qDebug() << "preview" << starttime.msecsTo(QTime::currentTime()) << size;
    //this->calculateTimer->start(0);
    this->processCalculate(size);
}
#include <QStack>

void ImageProcessor::showResults(QVector<int> sizes)
{
    QTime starttime = QTime::currentTime();
    QString result;
    result += tr("Number: ") + QString::number(sizes.size());
    int sum = 0;
    int min = INT_MAX;
    int max = 0;
    for (int i = 0; i < sizes.size(); i++)
    {
        sum += sizes.at(i);
        if (sizes.at(i) > max)
            max = sizes.at(i);
        if (sizes.at(i) < min)
            min = sizes.at(i);
    }
    result += "\n" + tr("Sum: ") + QString::number(sum);
    result += "\n" + tr("Min: ") + QString::number(min);
    result += "\n" + tr("Average: ") + QString::number((double)sum/sizes.size(), 'f', 2);
    result += "\n" + tr("Max: ") + QString::number(max);
    int area = this->image->width()*this->image->height();
    result += "\n" + tr("Occupied Area: ") + QString::number(((double)sum / area)*100, 'f', 2) + "%";
    result += "\n\n" + tr("Area size: ") + QString::number(area);
    this->resultsDockWidget->showResults(result);
    qDebug() << "showResults" << starttime.msecsTo(QTime::currentTime()) << sum;

}

void ImageProcessor::showFilteredResults(QVector<int> sizes)
{
    QTime starttime = QTime::currentTime();
    QString result;
    result += tr("Filtered Number: ") + QString::number(sizes.size());
    int sum = 0;
    int min = INT_MAX;
    int max = 0;
    for (int i = 0; i < sizes.size(); i++)
    {
        sum += sizes.at(i);
        if (sizes.at(i) > max)
            max = sizes.at(i);
        if (sizes.at(i) < min)
            min = sizes.at(i);
    }
    result += "\n" + tr("Filtered Sum: ") + QString::number(sum);
    result += "\n" + tr("Filtered Min: ") + QString::number(min);
    result += "\n" + tr("Filtered Average: ") + QString::number((double)sum/sizes.size(), 'f', 2);
    result += "\n" + tr("Filtered Max: ") + QString::number(max);int area = this->image->width()*this->image->height();
    result += "\n" + tr("Filtered Occupied Area: ") + QString::number(((double)sum / area)*100, 'f', 2) + "%";
    this->resultsDockWidget->showFilteredResults(result);
    qDebug() << "showFilteredResults" << starttime.msecsTo(QTime::currentTime()) << sum;
}

void ImageProcessor::hideFilteredResults()
{
    this->resultsDockWidget->showFilteredResults("");
}

#include <QApplication>
#include <QPoint>
#include <stdint.h>

void ImageProcessor::processCalculate(int sum)
{
    QTime starttime = QTime::currentTime();
    int width = this->image->width();
    //collectingpoints Нужно ещё оптимизировать.
    OnePointInfo* areasPoints = new OnePointInfo[sum]; //collectingpoints
    uint32_t globalindex = 0; //collectingpoints
    uint32_t pointindex = 0; //collectingpoints

    QList<int> sizes;

    for (int y = 0; y < image->height(); y++)
        for (int x = 0; x < image->width(); x++)
        {
            if (imageMatchedPixels[y*width + x] == (uchar)1)
            {
                int size = 0;
                QStack<QPoint> stack;
                stack.push(QPoint(x, y));
                while (!stack.isEmpty())
                {
                    QPoint point = stack.pop();
                    int y1 = point.y();
                    while (y1 >= 0 && (imageMatchedPixels[y1*width + point.x()] == (uchar)1))
                        y1 = y1 - 1;
                    y1 = y1 + 1;
                    int spanLeft = 0;
                    int spanRight = 0;
                    while (y1 < image->height() && (imageMatchedPixels[y1*width + point.x()] == (uchar)1))
                    {
                        imageMatchedPixels[y1*width + point.x()] = (uchar)2;
                        size++;
                        areasPoints[globalindex].x = point.x();
                        areasPoints[globalindex].y = y1;
                        areasPoints[globalindex].areaIndex = pointindex;
                        //MARK_POINT(globalindex, point.x(), y1, pointindex, areasPoints); //collectingpoints
                        globalindex++; //collectingpoints
                        ////////

                        if (spanLeft == 0 && point.x() > 0 && (imageMatchedPixels[y1*width + point.x()-1] == (uchar)1))
                        {
                            QPoint newpoint;
                            newpoint.setX(point.x()-1);
                            newpoint.setY(y1);
                            stack.push(newpoint);
                            spanLeft = 1;
                        }
                        else
                            if (spanLeft == 1 && point.x() > 0 && imageMatchedPixels[y1*width + point.x()-1] == 0)
                                spanLeft = 0;


                        if (spanRight == 0 && point.x() < image->width() - 1 && (imageMatchedPixels[y1*width + point.x()+1] == (uchar)1))
                        {
                            QPoint newpoint;
                            newpoint.setX(point.x()+1);
                            newpoint.setY(y1);
                            stack.push(newpoint);
                            spanRight= 1;
                        }
                        else
                        {
                            if (spanRight == 1 && point.x() < image->width() - 1 && imageMatchedPixels[y1*width + point.x()+1] == 0)
                                spanRight = 0;
                        }


                        y1 = y1 + 1;
                    }

                }
                sizes.append(size);
                pointindex++; //collectingpoints
            }

        }

    qDebug() << "calculate" << starttime.msecsTo(QTime::currentTime());
    //delete[] areasPoints;
    QVector<int> sizes_vector = sizes.toVector();
    this->showResults(sizes_vector);
    this->updateDisabledItemsOverlay(areasPoints, sum, sizes_vector);
}

void ImageProcessor::processMaskChange()
{
    QImage *qImageMask = new QImage(this->imageMaskLayer, image->width(), image->height(), image->width(), QImage::Format_Indexed8);
    qImageMask->setColor(0, (uint)0x00000000);
    qImageMask->setColor(1, (uint)0xAA00FF00);
    this->layerManager->updateImageMask(qImageMask);
    delete qImageMask;
    this->processDataChange();
}

void ImageProcessor::drawCircleOnMask(int x, int y, int radius, uchar value)
{
    QTime starttime = QTime::currentTime();
    qDebug() << "drawCircleOnArray";
    this->drawCircleOnArray(this->imageMaskLayer, x, y, this->image->width(), this->image->height(), radius, value);
    qDebug() << "drawCircleOnMask" << starttime.msecsTo(QTime::currentTime());
    this->processMaskChange();
}

void ImageProcessor::drawCircleOnArray(uchar *array, int x, int y, int width, int height, int radius, uchar value)
{
    int xStart = 0;
    if (x - radius > 0)
        xStart = x - radius;

    int yStart = 0;
    if (y - radius > 0)
        yStart = y - radius;

    int xEnd = width - 1;
    if (x + radius < width)
        xEnd = x + radius;

    int yEnd = height - 1;
    if (y + radius < height)
        yEnd = y + radius;

    qDebug() << xStart << xEnd << yStart << yEnd;

    for (int yc = yStart; yc <= yEnd; yc++)
        for (int xc = xStart; xc <= xEnd; xc++)
            if (sqrt(SQR(yc - y) + SQR(xc - x)) <= radius)
                array[yc*width + xc] = (uchar)value;
}
void ImageProcessor::drawMask(QPointF point)
{
    this->drawCircleOnMask(point.x(), point.y(), this->toolsDockWidget->maskBrushSize(), (uchar)1);
}

void ImageProcessor::eraseMask(QPointF point)
{
    this->drawCircleOnMask(point.x(), point.y(), this->toolsDockWidget->maskBrushSize(), (uchar)0);
}

void ImageProcessor::updateBrushPosition(QPointF point)
{
    this->layerManager->updateBrushLayerPosition(point.toPoint());
}

#define IS_SIZE_BAD(size, min, max) (((size) < (min)) || ((size) > (max)))

void ImageProcessor::updateDisabledItemsOverlay(OnePointInfo *areasPoints, int size, QVector<int> sizes)
{
    QTime starttime = QTime::currentTime();
    int *sizes_c_array = sizes.data();
    int min = this->toolsDockWidget->minSize();
    int max = this->toolsDockWidget->maxSize();
    uint32_t pixelsInImage = this->image->width()*this->image->height();
    uint32_t points = sizes.size();
    if (this->toolsDockWidget->isSizeRangeEnabled())
    {
        QList<int> sizes_filtered;
        int width = this->image->width();
        for (int i = 0; i < pixelsInImage; i++)
            this->imageDisabledItemsOverlay[i] = (uchar)0;
        for (int i = 0; i < size; i++)
        {
            if (IS_SIZE_BAD(sizes_c_array[areasPoints[i].areaIndex], min, max))
                this->imageDisabledItemsOverlay[areasPoints[i].y*width + areasPoints[i].x] = (uchar)1;
        }

        for (int i = 0; i < points; i++)
        {
            if (!IS_SIZE_BAD(sizes_c_array[i], min, max))
            {
                sizes_filtered.append(sizes_c_array[i]);
            }
        }


        QImage *imageOverlay = new QImage(this->imageDisabledItemsOverlay, image->width(), image->height(), image->width(), QImage::Format_Indexed8);
        imageOverlay->setColor(0, (uint)0x00000000);
        imageOverlay->setColor(1, (uint)0xAA444444);
        this->layerManager->updateDisabletItemsOverlayLayer(imageOverlay);
        this->showFilteredResults(sizes_filtered.toVector());
        delete imageOverlay;
        delete[] areasPoints;
    }
    else
    {
        for (int i = 0; i < pixelsInImage; i++)
            this->imageDisabledItemsOverlay[i] = (uchar)0;
        QImage *imageOverlay = new QImage(this->imageDisabledItemsOverlay, image->width(), image->height(), image->width(), QImage::Format_Indexed8);
        imageOverlay->setColor(0, (uint)0x00000000);
        this->layerManager->updateDisabletItemsOverlayLayer(imageOverlay);
        delete imageOverlay;
        this->hideFilteredResults();
    }
    qDebug() << "updateDisabledItemsOverlay" << starttime.msecsTo(QTime::currentTime());
}

void ImageProcessor::on_toolsDockWidget_drawMask()
{
    this->graphicsView->disconnect(this);
    this->graphicsView->setDragMode(QGraphicsView::NoDrag);
    this->connect(this->graphicsView,
                  SIGNAL(mousePressed(QPointF)), this, SLOT(drawMask(QPointF)));
    this->connect(this->graphicsView,
                  SIGNAL(mouseMoved(QPointF)), this, SLOT(drawMask(QPointF)));
    //Making brush
/*
    QImage* brush = this->generateBrushImage();
    this->layerManager->updateBrush(QPoint(0, 0), brush);
    delete brush;

    this->connect(this->graphicsView,
                  SIGNAL(mouseMoved(QPointF)), this, SLOT(updateBrushPosition(QPointF)));
*/
}

void ImageProcessor::on_toolsDockWidget_eraseMask()
{
    this->graphicsView->disconnect(this);
    this->graphicsView->setDragMode(QGraphicsView::NoDrag);
    this->connect(this->graphicsView,
                  SIGNAL(mousePressed(QPointF)), this, SLOT(eraseMask(QPointF)));
    this->connect(this->graphicsView,
                  SIGNAL(mouseMoved(QPointF)), this, SLOT(eraseMask(QPointF)));
/*    QImage* brush = this->generEraseateBrushImage();
    this->layerManager->updateBrush(QPoint(0, 0), brush);
    delete brush;

    this->connect(this->graphicsView,
                  SIGNAL(mouseMoved(QPointF)), this, SLOT(updateBrushPosition(QPointF)));
*/
}

void ImageProcessor::on_toolsDockWidget_pickColor()
{
    this->graphicsView->disconnect(this);
    this->graphicsView->setDragMode(QGraphicsView::NoDrag);
    this->connect(this->graphicsView,
                  SIGNAL(mousePressed(QPointF)), this, SLOT(pick(QPointF)));
}


void ImageProcessor::on_toolsDockWidget_move()
{
    this->graphicsView->disconnect(this);
    this->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
}

void ImageProcessor::on_toolsDockWidget_completelyEraseMask()
{
    for (int i = 0; i < this->imageNumberOfPixels; i++)
        this->imageMaskLayer[i] = (uchar)0;
    this->processMaskChange();
}

void ImageProcessor::on_toolsDockWidget_completelyFillMask()
{
    for (int i = 0; i < this->imageNumberOfPixels; i++)
        this->imageMaskLayer[i] = (uchar)1;
    this->processMaskChange();
}

void ImageProcessor::on_toolsDockWidget_sizeRangeChanged()
{
    this->processDataChange();
}

void ImageProcessor::on_toolsDockWidget_sizeRangeEnabledChanged()
{
    this->processDataChange();
}


QWidget *ImageProcessor::getMainWidget()
{
    return this->graphicsView;
}


void ImageProcessor::disableResults()
{
    this->resultsDockWidget->disableResults();
}

QImage *ImageProcessor::generateBrushImage()
{
    int radius = this->toolsDockWidget->maskBrushSize();
    uchar * brushImageArray = new uchar[SQR(radius*2)];

    //Заполнение 0-ми. Медленно, костыльно, но работает и быстро реализовано. //TODO: переписать
    for (int i = 0; i < SQR(radius*2); i++)
        brushImageArray[i] = (uchar)(0);

    this->drawCircleOnArray(brushImageArray, radius, radius, radius*2, radius*2, radius, (uchar)1);

    QImage *brushImage = new QImage(brushImageArray, radius*2, radius*2, radius*2, QImage::Format_Indexed8);
    brushImage->setColor(0, (uint)0x00000000);
    brushImage->setColor(1, (uint)0xEEFFFFFF);

    //delete brushImageArray;

    return brushImage;

}
