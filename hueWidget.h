#ifndef HUEWIDGET_H
#define HUEWIDGET_H

#include "opencv2/core/core.hpp"
#include <opencv2/opencv.hpp>

#include "colorEdgesDetection.h"

#include <QFileDialog>
#include <QImage>
#include <QGraphicsPixmapItem>

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QSpinBox>

#include "graphicsview.h"
#include "rangecontrol.h"
#include <QtGui/QTabBar>
#include <QTimer>
#include <QVector>

#include <QDebug>
#include "image.h"
#include "resultinfo.h"

using namespace cv;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();

private:
    Image* image;
    QVBoxLayout* layout;
    QHBoxLayout* buttonsLayout;
    QVBoxLayout* colorRangeLayout;

    QPushButton* openImageButton;
    QPushButton* calculateButton;
    QPushButton* selectionButton;
    QPushButton* saveSel;
    QPushButton* pickButton;
    QPushButton* selectionOnes;
    QPushButton* positiveNegativePainting;
    QPushButton* saveCRange;
    QPushButton* savemisc;
    QPushButton* saveall;
    QSpinBox* selectionBrushSize;
    QSpinBox* min;
    QSpinBox* max;

    bool picking;



    QGraphicsScene* graphicsScene;
    GraphicsView* graphicsView;
    QTabBar* tabBar;
    QTimer* timer;

    Mat originalImage;
    Mat* selectionMask;
    Mat* rasterMask;

    QGraphicsPixmapItem* originalImageItem;
    QGraphicsPixmapItem* maskItem;
    QGraphicsPixmapItem* selMaskItem;

    QList<RangeControl*> rangeControls;

    bool selecting;
    std::vector<QGraphicsEllipseItem*> selectionPoints;

    void updateGraphicsViewFromMat(int index = 0, bool inactive = false);
    void updateSelectionMask();

    QVector<QRgb> *colorTable;
    QVector<QRgb> *inactiveColorTable;
    QVector<QRgb> *itemColorTable;
    QVector<QRgb> *selectionColorTable;

    std::vector<QGraphicsPixmapItem*> maskItems;
    std::vector<size_t>* edges;

    void clearItems();

    bool isUpToDate;
    bool drawing;
    uchar sel;


private slots:
    void openNewImage();
    void calculate();
    void updateRasterMask();
    void updateItemBasedMask();
    void switchTab(int);
    void selectionButtonClicked();
    void newPoint(QPointF);

    void pickStart();
    void pick(QPointF);

    void selectionOnesClicked();
    void invertselpainting();

    void rangeChanged();

    void saveSelection();
    void saveMisc();
    void saveColorRange();
    void saveAll();
};

#endif // HUEWIDGET_H
