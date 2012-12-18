#include "imageprocessor.h"
#include <QDebug>
#include <QTime>

#define DRAW_MASK_BRUSH_COLOR 0x60000000
#define ERASE_MASK_BRUSH_COLOR 0x50FFFFFF
#define MASK_COLOR 0xBA808080
#define MATCHED_COLOR 0xFF00FFFF
#define FILTERED_COLOR 0xAA444444

#define SQR(s)  ((s) * (s))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

ImageProcessor::ImageProcessor(QString fileName, QObject *parent) :
    QObject(parent)
{
    this->sourceImage.load(fileName);
    this->imageNumberOfPixels = this->sourceImage.width()*this->sourceImage.height();
    this->imageMatchedPixels.resize(imageNumberOfPixels);
    this->imageMaskLayer.resize(imageNumberOfPixels);
    this->imageDisabledItemsOverlay.resize(imageNumberOfPixels);
    //converting to HSV
    this->H.resize(imageNumberOfPixels);
    this->S.resize(imageNumberOfPixels);
    this->V.resize(imageNumberOfPixels);
    //converting to HSV
    {
        QTime starttime = QTime::currentTime();
        const uchar * rawData = sourceImage.bits();

        QRgb* source = (QRgb*)rawData;

        for (int i = 0; i < imageNumberOfPixels; i++)
        {
            float R = (qRed(source[i])*1.0)/255;
            float G = (qGreen(source[i])*1.0)/255;
            float B = (qBlue(source[i])*1.0)/255;

            float Max = MAX(R, MAX(G, B));
            float Min = MIN(R, MIN(G, B));

            //getting H
            if (Max == Min)
            {
                H[i] = 0;
            }
            else if (Max == R && G >= B)
            {
                H[i] = (60*( (G - B)/(Max - Min) )) + 0;
            }
            else if (Max == R && G < B)
            {
                H[i] = (60*( (G - B)/(Max - Min) )) + 360;
            }
            else if (Max == G)
            {
                H[i] = (60*( (B - R)/(Max - Min) )) + 120;
            }
            else if (Max == B)
            {
                H[i] = (60*( (R - G)/(Max - Min) )) + 240;
            }
            //getting S
            if (Max == 0)
            {
                S[i] = 0;
            }
            else
            {
                S[i] = Min/Max;
            }
            //getting V
            V[i] = Max;
        }
        qDebug() << "HSV" << starttime.msecsTo(QTime::currentTime());
    }
    this->graphicsView = new GraphicsView();
    this->graphicsView->setScene(&graphicsScene);
    this->graphicsView->setSceneRect(0, 0, this->sourceImage.width(), this->sourceImage.height());
    connect(graphicsView, SIGNAL(mouseMovedWhileLeftMouseButtonNotPressed(QPoint)), this, SLOT(hover(QPoint)));
    connect(graphicsView, SIGNAL(mouseEntered()), this, SLOT(on_graphicsView_mouseEntered()));
    connect(graphicsView, SIGNAL(mouseLeaved()), this, SLOT(on_graphicsView_mouseLeaved()));
    this->colorDockWidget = new ColorDockWidget();
    this->layerManager = new LayerDockWidget(&graphicsScene);
    this->resultsDockWidget = new ResultsDockWidget();
    this->toolsDockWidget = new ToolsDockWidget(this->imageNumberOfPixels, (this->sourceImage.width() > this->sourceImage.height()) ? (this->sourceImage.width()) : (this->sourceImage.height()));
    this->toolsDockWidget->setObjectName("toolsDockWidget");
    this->toolsDockWidget->QObject::setParent(this);
    this->layerManager->updateBaseImage(&sourceImage);
    this->previewTimer.setSingleShot(true);
    this->calculateTimer.setSingleShot(true);
    this->connect(&previewTimer, SIGNAL(timeout()), this, SLOT(processPreview()));
    //this->connect(&calculateTimer, SIGNAL(timeout()), this, SLOT(processCalculate()));

    this->connect(this->colorDockWidget, SIGNAL(colorOrDeltaChanged()), this, SLOT(processDataChange()));

    QMetaObject::connectSlotsByName(this);
}

ImageProcessor::~ImageProcessor()
{
    delete this->colorDockWidget;
    delete this->layerManager;
    delete this->toolsDockWidget;
}

QList<QDockWidget *> ImageProcessor::getDockWidgets()
{
    QList<QDockWidget *> docks;
    docks.append(this->colorDockWidget);
    //docks.append(this->layerManager);
    docks.append(this->toolsDockWidget);
    docks.append(this->resultsDockWidget);
    return docks;
}

void ImageProcessor::pick(QPoint point)
{
    //DONE: предусмотреть вариант клика вне картинки.
    //TODO: подумать, что делать при клике вне картинки.
    //DONE: предусмотреть клик по пикселю с альфа-каналом. (надо ли?) - не надо.
    qDebug() << point << sourceImage.size();
    if (point.x() >= 0 && point.y() >= 0 && point.x() < sourceImage.width() && point.y() < sourceImage.height())
    {
        if (colorDockWidget->method() == 0)
            this->colorDockWidget->setColor(this->sourceImage.pixel(point));
        else if (colorDockWidget->method() == 1)
        {
            double h = H[point.y()*sourceImage.width() + point.x()];
            double s = S[point.y()*sourceImage.width() + point.x()];
            double v = V[point.y()*sourceImage.width() + point.x()];

            colorDockWidget->setLowerLimit(0, h - 15);
            colorDockWidget->setUpperLimit(0, h + 15);
            colorDockWidget->setLowerLimit(1, s - 0.05);
            colorDockWidget->setUpperLimit(1, s + 0.05);
            colorDockWidget->setLowerLimit(2, v - 0.05);
            colorDockWidget->setUpperLimit(2, v + 0.05);
        }
    }
    else
        qDebug() << "ВНЕ КАРТИНКИ";
}
#include <QApplication>
void ImageProcessor::processDataChange()
{
    this->previewTimer.stop();
    this->previewTimer.start(0);
    //this->calculateTimer->stop();
    //this->disableResults();
}

#include <QDebug>
#include <cmath>
#include <QTime>
/*
#define RED(QRgb) ((uchar*)(&QRgb))[1]
#define GREEN(QRgb) ((uchar*)(&QRgb))[2]
#define BLUE(QRgb) ((uchar*)(&QRgb))[3]
*/
#define RED(QRgb) qRed(QRgb)
#define GREEN(QRgb) qGreen(QRgb)
#define BLUE(QRgb) qBlue(QRgb)

#define CIE76_RGB(x, y) sqrt(SQR(RED(x)-RED(y))+SQR(GREEN(x)-GREEN(y))+SQR(BLUE(x)-BLUE(y)))
void ImageProcessor::processPreview()
{
    QTime starttime = QTime::currentTime();
    int size = 0;

    QImage *image = &sourceImage;
    const uchar * rawData = image->bits();
    QRgb*  source = (QRgb*)rawData;

    if (colorDockWidget->method() == 0)
    {
    QRgb  baseColor = this->colorDockWidget->rgb();
    double maxDelta = this->colorDockWidget->getDelta();

    for (int i = 0; i < imageNumberOfPixels; i++)
    {
        size += imageMatchedPixels[i] = (uchar)((imageMaskLayer[i] == 0) && (CIE76_RGB(source[i], baseColor) < maxDelta));
    }

    }
    else if (colorDockWidget->method() == 1)
    {
        float Hmin = colorDockWidget->lowerLimit(0);
        float Hmax = colorDockWidget->upperLimit(0);
        float Smin = colorDockWidget->lowerLimit(1);
        float Smax = colorDockWidget->upperLimit(1);
        float Vmin = colorDockWidget->lowerLimit(2);
        float Vmax = colorDockWidget->upperLimit(2);

        if (Hmin <= Hmax)
        {
            for (int i = 0; i < imageNumberOfPixels; i++)
            size += imageMatchedPixels[i] = (uchar)((imageMaskLayer[i] == 0) && (H[i] >= Hmin) && (H[i] <= Hmax) && (S[i] >= Smin) && (S[i] <= Smax) && (V[i] >= Vmin) && (V[i] <= Vmax) );
        }
        else
        {
            for (int i = 0; i < imageNumberOfPixels; i++)
            size += imageMatchedPixels[i] = (uchar)((imageMaskLayer[i] == 0) && ((H[i] >= Hmin) || (H[i] <= Hmax)) && (S[i] >= Smin) && (S[i] <= Smax) && (V[i] >= Vmin) && (V[i] <= Vmax) );
        }
    }

    QImage *qImageMatchedPixels = new QImage(imageMatchedPixels.data(), image->width(), image->height(), image->width(), QImage::Format_Indexed8);
    qImageMatchedPixels->setColor(0, (uint)0x00000000);
    qImageMatchedPixels->setColor(1, (uint)MATCHED_COLOR);

    this->layerManager->updateColorMask(qImageMatchedPixels);
    delete qImageMatchedPixels;

    qDebug() << "preview" << starttime.msecsTo(QTime::currentTime()) << size;
    //this->calculateTimer->start(0);
    qApp->processEvents();
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
    int area = this->sourceImage.width()*this->sourceImage.height();
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
    result += "\n" + tr("Filtered Max: ") + QString::number(max);int area = this->sourceImage.width()*this->sourceImage.height();
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
    int width = this->sourceImage.width();
    //collectingpoints Нужно ещё оптимизировать.

    if (this->property("areasPoints").isValid())
        delete[] (OnePointInfo*)(this->property("areasPoints").value<void*>());

    OnePointInfo* areasPoints = new OnePointInfo[sum]; //collectingpoints
    uint32_t globalindex = 0; //collectingpoints
    uint32_t pointindex = 0; //collectingpoints

    QList<int> sizes;

    for (int y = 0; y < sourceImage.height(); y++)
        for (int x = 0; x < sourceImage.width(); x++)
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
                    while (y1 < sourceImage.height() && (imageMatchedPixels[y1*width + point.x()] == (uchar)1))
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


                        if (spanRight == 0 && point.x() < sourceImage.width() - 1 && (imageMatchedPixels[y1*width + point.x()+1] == (uchar)1))
                        {
                            QPoint newpoint;
                            newpoint.setX(point.x()+1);
                            newpoint.setY(y1);
                            stack.push(newpoint);
                            spanRight= 1;
                        }
                        else
                        {
                            if (spanRight == 1 && point.x() < sourceImage.width() - 1 && imageMatchedPixels[y1*width + point.x()+1] == 0)
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
    this->setProperty("areasPoints", qVariantFromValue((void*)areasPoints));
    this->setProperty("areasNumber", qVariantFromValue(sum));
    QVector<int> sizes_vector = sizes.toVector();
    this->showResults(sizes_vector);
    this->updateDisabledItemsOverlay(areasPoints, sum, sizes_vector);
}

void ImageProcessor::processMaskChange()
{
    QImage *qImageMask = new QImage(this->imageMaskLayer.data(), sourceImage.width(), sourceImage.height(), sourceImage.width(), QImage::Format_Indexed8);
    qImageMask->setColor(0, (uint)0x00000000);
    qImageMask->setColor(1, (uint)MASK_COLOR);
    this->layerManager->updateImageMask(qImageMask);
    delete qImageMask;
    this->previewTimer.stop();
    this->previewTimer.start(50);
   // qApp->processEvents();
    //this->processDataChange();
}

void ImageProcessor::drawCircleOnMask(int x, int y, int radius, uchar value)
{
    QTime starttime = QTime::currentTime();
    this->drawCircleOnArray(this->imageMaskLayer.data(), x, y, this->sourceImage.width(), this->sourceImage.height(), radius, value);
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

    for (int yc = yStart; yc <= yEnd; yc++)
        for (int xc = xStart; xc <= xEnd; xc++)
            if (sqrt(SQR(yc - y) + SQR(xc - x)) <= radius)
                array[yc*width + xc] = (uchar)value;
}
void ImageProcessor::drawMask(QPoint point)
{
    this->drawCircleOnMask(point.x(), point.y(), this->toolsDockWidget->maskBrushSize(), (uchar)1);
}

void ImageProcessor::eraseMask(QPoint point)
{
    this->drawCircleOnMask(point.x(), point.y(), this->toolsDockWidget->maskBrushSize(), (uchar)0);
}

void ImageProcessor::updateBrushPosition(QPoint point)
{
    this->layerManager->updateBrushLayerPosition(point);
}

#define IS_SIZE_BAD(size, min, max) (((size) < (min)) || ((size) > (max)))

void ImageProcessor::updateDisabledItemsOverlay(OnePointInfo *areasPoints, int size, QVector<int> sizes)
{
    QTime starttime = QTime::currentTime();
    int *sizes_c_array = sizes.data();
    int min = this->toolsDockWidget->minSize();
    int max = this->toolsDockWidget->maxSize();
    uint32_t pixelsInImage = this->sourceImage.width()*this->sourceImage.height();
    uint32_t points = sizes.size();
    if (this->toolsDockWidget->isSizeRangeEnabled())
    {
        QList<int> sizes_filtered;
        int width = this->sourceImage.width();
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


        QImage *imageOverlay = new QImage(this->imageDisabledItemsOverlay.data(), sourceImage.width(), sourceImage.height(), sourceImage.width(), QImage::Format_Indexed8);
        imageOverlay->setColor(0, (uint)0x00000000);
        imageOverlay->setColor(1, (uint)FILTERED_COLOR);
        this->layerManager->updateDisabletItemsOverlayLayer(imageOverlay);
        this->showFilteredResults(sizes_filtered.toVector());
        delete imageOverlay;
    }
    else
    {
        for (int i = 0; i < pixelsInImage; i++)
            this->imageDisabledItemsOverlay[i] = (uchar)0;
        QImage *imageOverlay = new QImage(this->imageDisabledItemsOverlay.data(), sourceImage.width(), sourceImage.height(), sourceImage.width(), QImage::Format_Indexed8);
        imageOverlay->setColor(0, (uint)0x00000000);
        this->layerManager->updateDisabletItemsOverlayLayer(imageOverlay);
        delete imageOverlay;
        this->hideFilteredResults();
    }
    qDebug() << "updateDisabledItemsOverlay" << starttime.msecsTo(QTime::currentTime());
}

void ImageProcessor::on_toolsDockWidget_drawMask()
{
    //removing all shit
    this->graphicsView->disconnect(this, SLOT(eraseMask(QPoint)));
    this->graphicsView->disconnect(this, SLOT(drawMask(QPoint)));
    this->graphicsView->disconnect(this, SLOT(pick(QPoint)));
    this->graphicsView->disconnect(this, SLOT(updateBrushPosition(QPoint)));
    this->graphicsView->disconnect(this, SLOT(hover(QPoint)));
    this->graphicsView->setDragMode(QGraphicsView::NoDrag);


    this->connect(this->graphicsView,
                  SIGNAL(mousePressed(QPoint)), this, SLOT(drawMask(QPoint)));
    this->connect(this->graphicsView,
                  SIGNAL(mouseMovedWhileLeftMouseButtonPressed(QPoint)), this, SLOT(drawMask(QPoint)));
    //Making brush

    this->layerManager->updateBrush(QPoint(-1, -1), this->generateBrushImage(DRAW_MASK_BRUSH_COLOR));

    this->connect(this->graphicsView,
                  SIGNAL(mouseMoved(QPoint)), this, SLOT(updateBrushPosition(QPoint)));

}

void ImageProcessor::on_toolsDockWidget_eraseMask()
{
    //removing all shit
    this->graphicsView->disconnect(this, SLOT(eraseMask(QPoint)));
    this->graphicsView->disconnect(this, SLOT(drawMask(QPoint)));
    this->graphicsView->disconnect(this, SLOT(pick(QPoint)));
    this->graphicsView->disconnect(this, SLOT(updateBrushPosition(QPoint)));
    this->graphicsView->disconnect(this, SLOT(hover(QPoint)));
    this->graphicsView->setDragMode(QGraphicsView::NoDrag);


    this->connect(this->graphicsView,
                  SIGNAL(mousePressed(QPoint)), this, SLOT(eraseMask(QPoint)));
    this->connect(this->graphicsView,
                  SIGNAL(mouseMovedWhileLeftMouseButtonPressed(QPoint)), this, SLOT(eraseMask(QPoint)));


    this->layerManager->updateBrush(QPoint(-1, -1), this->generateBrushImage(ERASE_MASK_BRUSH_COLOR));
    this->connect(this->graphicsView,
                  SIGNAL(mouseMoved(QPoint)), this, SLOT(updateBrushPosition(QPoint)));
}

void ImageProcessor::on_toolsDockWidget_pickColor()
{
    //removing all shit
    this->graphicsView->disconnect(this, SLOT(eraseMask(QPoint)));
    this->graphicsView->disconnect(this, SLOT(drawMask(QPoint)));
    this->graphicsView->disconnect(this, SLOT(pick(QPoint)));
    this->graphicsView->disconnect(this, SLOT(updateBrushPosition(QPoint)));
    this->graphicsView->disconnect(this, SLOT(hover(QPoint)));
    this->graphicsView->setDragMode(QGraphicsView::NoDrag);

    connect(this->graphicsView, SIGNAL(mouseMovedWhileLeftMouseButtonNotPressed(QPoint)), this, SLOT(hover(QPoint)));
    this->connect(this->graphicsView,
                  SIGNAL(mousePressed(QPoint)), this, SLOT(pick(QPoint)));
}


void ImageProcessor::on_toolsDockWidget_move()
{
    //removing all shit
    this->graphicsView->disconnect(this, SLOT(eraseMask(QPoint)));
    this->graphicsView->disconnect(this, SLOT(drawMask(QPoint)));
    this->graphicsView->disconnect(this, SLOT(pick(QPoint)));
    this->graphicsView->disconnect(this, SLOT(updateBrushPosition(QPoint)));
    this->graphicsView->disconnect(this, SLOT(hover(QPoint)));
    this->graphicsView->setDragMode(QGraphicsView::NoDrag);

    connect(this->graphicsView, SIGNAL(mouseMovedWhileLeftMouseButtonNotPressed(QPoint)), this, SLOT(hover(QPoint)));
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

void ImageProcessor::on_toolsDockWidget_maskBrushSizeChanged()
{
    if (toolsDockWidget->isDrawingMask())
        this->layerManager->updateBrush(QPoint(-1, -1), this->generateBrushImage(DRAW_MASK_BRUSH_COLOR));
    if (toolsDockWidget->isErasingMask())
        this->layerManager->updateBrush(QPoint(-1, -1), this->generateBrushImage(ERASE_MASK_BRUSH_COLOR));
}

void ImageProcessor::on_toolsDockWidget_sizeRangeChanged()
{
    this->processDataChange();
}

void ImageProcessor::on_toolsDockWidget_sizeRangeEnabledChanged()
{
    this->processDataChange();
}

void ImageProcessor::on_graphicsView_mouseLeaved()
{
    if ((toolsDockWidget->isDrawingMask()) || (toolsDockWidget->isErasingMask()))
        layerManager->updateBrushVisibility(false);
}

void ImageProcessor::on_graphicsView_mouseEntered()
{
    if ((toolsDockWidget->isDrawingMask()) || (toolsDockWidget->isErasingMask()))
        layerManager->updateBrushVisibility(true);
}

void ImageProcessor::hover(QPoint point)
{
    QPoint p;
    p.setX(point.x());
    p.setY(point.y());
    if (!(this->property("lastPointHasInfo").isValid()) || this->property("lastPointHasInfo").toPoint() != p)
    {
        this->setProperty("lastPointHasInfo", QVariant(p));
        if (this->property("areasPoints").isValid() && this->property("areasNumber").isValid())
        {
            OnePointInfo* areasPoints = (OnePointInfo*)(this->property("areasPoints").value<void*>());
            int areasNumber = this->property("areasNumber").toInt();
            int size = 0;
            bool indexgot = false;
            uint32_t index;
            for (int i = 0; i < areasNumber; i++)
            {
                if ((p.x() == areasPoints[i].x) && (p.y() == areasPoints[i].y))
                {
                    index = areasPoints[i].areaIndex;
                    indexgot = true;
                    break;
                }
            }
            if (indexgot)
                for (int i = 0; i < areasNumber; i++)
                {
                    if (index == areasPoints[i].areaIndex)
                        size++;
                }
            emit notifyHoverItemSize(size);
        }
    }
}


QWidget *ImageProcessor::getMainWidget()
{
    return this->graphicsView;
}


void ImageProcessor::disableResults()
{
    this->resultsDockWidget->disableResults();
}
#include <vector>
QGraphicsPixmapItem *ImageProcessor::generateBrushImage(QRgb color)
{
    int radius = this->toolsDockWidget->maskBrushSize();
    int size = SQR(radius*2 + 1);
    std::vector<uchar> brushImageArray(size, 0);

    this->drawCircleOnArray(brushImageArray.data(), radius, radius, radius*2 + 1, radius*2 +1, radius, (uchar)1);

    QImage brushImage(brushImageArray.data(), radius*2 + 1, radius*2 + 1, radius*2 + 1, QImage::Format_Indexed8);
    brushImage.setColor(0, (uint)0x00000000);
    brushImage.setColor(1, (uint)color);

    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(brushImage));
    pixmapItem->setOffset(0-radius, 0-radius);
    pixmapItem->setVisible(false);

    return pixmapItem;
}
