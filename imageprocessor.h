#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>

#include <QString>
#include <QImage>
#include <QBitmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QPointF>
#include <QTimer>
#include "graphicsview.h"
#include "qgraphicsviewextended.h"
#include "colordockwidget.h"
#include "layerdockwidget.h"
#include "layerlistmodel.h"
#include "resultsdockwidget.h"
#include "toolsdockwidget.h"
#include <QList>
#include <QVector>
#include <stdint.h>
#include <limits.h>

typedef struct _OnePointInfo
{
    uint16_t x;
    uint16_t y;
    uint32_t areaIndex;
} OnePointInfo;

class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QString fileName, QObject *parent = 0);
    ~ImageProcessor();
    QList<QDockWidget*> getDockWidgets();
    QWidget *getMainWidget();
    
signals:
    
public slots:
    void pick(QPointF point);
    void processDataChange();
    void processPreview();
    void processCalculate(int sum);
    void processMaskChange();
    void drawCircleOnMask(int x, int y, int radius, uchar value);
    void drawCircleOnArray(uchar *array, int x, int y, int width, int height, int radius, uchar value);
    void drawMask(QPointF point);
    void eraseMask(QPointF point);
    void updateBrushPosition(QPointF point);
    void updateDisabledItemsOverlay(OnePointInfo* areasPoints, int size, QVector<int> sizes);
    void on_toolsDockWidget_drawMask();
    void on_toolsDockWidget_eraseMask();
    void on_toolsDockWidget_pickColor();
    void on_toolsDockWidget_move();
    void on_toolsDockWidget_completelyEraseMask();
    void on_toolsDockWidget_completelyFillMask();
    void on_toolsDockWidget_sizeRangeChanged();
    void on_toolsDockWidget_sizeRangeEnabledChanged();

private:
    QImage *image;
    unsigned int imageNumberOfPixels;
    uchar* imageMatchedPixels;
    uchar* imageMaskLayer;
    uchar* imageDisabledItemsOverlay;
    GraphicsView *graphicsView; //Ui
    ColorDockWidget *colorDockWidget; //Ui
    LayerDockWidget *layerManager; //Ui
    ResultsDockWidget *resultsDockWidget; //Ui
    ToolsDockWidget *toolsDockWidget;
    QGraphicsScene *graphicsScene;
    QTimer *previewTimer;
    QTimer *calculateTimer;
    void showResults(QVector<int> sizes);
    void showFilteredResults(QVector<int> sizes);
    void hideFilteredResults();
    void disableResults();
    QPointF lastPoint;
    QImage *generateBrushImage();
};

#endif // IMAGEPROCESSOR_H
