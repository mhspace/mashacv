#include "hueWidget.h"
#include <math.h>
#include <qapplication.h>
#include <fstream>
#define pointRadius 10
#define colordelta 40

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    this->layout = new QVBoxLayout();
    this->buttonsLayout = new QHBoxLayout();

    this->openImageButton = new QPushButton(QString::fromUtf8("ПЫЩЬ"));//Load Image");
    this->calculateButton = new QPushButton(QString::fromUtf8("ПЫЩЬ"));//Calculate");
    this->selectionButton = new QPushButton(QString::fromUtf8("Paint mask"));
    this->pickButton = new QPushButton(QString::fromUtf8("Pick"));
    this->positiveNegativePainting = new QPushButton(QString::fromUtf8("Set negative"));
    this->saveSel = new QPushButton(QString::fromUtf8("Save selection"));
    this->saveCRange = new QPushButton(QString::fromUtf8("Save color range"));
    this->savemisc = new QPushButton(QString::fromUtf8("Save misc"));
    this->saveall = new QPushButton(QString::fromUtf8("Save all"));
    this->connect(this->openImageButton, SIGNAL(clicked()), this, SLOT(openNewImage()));
    this->connect(this->calculateButton, SIGNAL(clicked()), this, SLOT(calculate()));
    this->connect(this->selectionButton, SIGNAL(clicked()), this, SLOT(selectionButtonClicked()));
    this->connect(this->pickButton, SIGNAL(clicked()), this, SLOT(pickStart()));
    this->connect(this->positiveNegativePainting, SIGNAL(clicked()), this, SLOT(invertselpainting()));
    this->connect(this->saveSel, SIGNAL(clicked()), this, SLOT(saveSelection()));
    this->connect(this->saveCRange, SIGNAL(clicked()), this, SLOT(saveColorRange()));
    this->connect(this->savemisc, SIGNAL(clicked()), this, SLOT(saveMisc()));
    this->connect(this->saveall, SIGNAL(clicked()), this, SLOT(saveAll()));
    this->sel = (uchar)1;
    this->selectionBrushSize = new QSpinBox();
    this->selectionBrushSize->setValue(pointRadius);
    this->min = new QSpinBox();
    this->min->setRange(0, pow(2, (sizeof(int)*8 - 1)) - 1);
    this->min->setValue(0);
    this->max = new QSpinBox();
    this->max->setRange(-1, pow(2, (sizeof(int)*8 - 1)) - 1);
    this->max->setValue(-1);

    this->connect(this->min, SIGNAL(valueChanged(int)), this, SLOT(rangeChanged()));
    this->connect(this->max, SIGNAL(valueChanged(int)), this, SLOT(rangeChanged()));

    this->tabBar = new QTabBar();
    this->tabBar->addTab("Original Image");
    this->tabBar->addTab("Color range mask");
    this->tabBar->addTab("Selection mask");

    this->connect(this->tabBar, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));

    this->graphicsScene = new QGraphicsScene();
    this->graphicsView = new GraphicsView(this->graphicsScene);

    //this->buttonsLayout->addWidget(this->openImageButton);
    this->buttonsLayout->addWidget(this->calculateButton);
    this->buttonsLayout->addWidget(this->selectionButton);
    this->buttonsLayout->addWidget(this->selectionBrushSize);
    this->buttonsLayout->addWidget(this->positiveNegativePainting);
    this->buttonsLayout->addWidget(this->saveSel);
    this->buttonsLayout->addWidget(this->pickButton);
    this->buttonsLayout->addWidget(this->min);
    this->buttonsLayout->addWidget(this->max);
    this->buttonsLayout->addWidget(this->saveCRange);
    this->buttonsLayout->addWidget(this->savemisc);
    this->buttonsLayout->addWidget(this->saveall);

    this->layout->addLayout(this->buttonsLayout);
    this->colorRangeLayout = new QVBoxLayout();
    this->layout->addLayout(this->colorRangeLayout);
    //this->layout->addWidget(this->tabBar);
    this->layout->addWidget(this->graphicsView);

    this->setLayout(layout);

    this->originalImageItem = new QGraphicsPixmapItem();
    this->maskItem = new QGraphicsPixmapItem();
    this->selMaskItem = new QGraphicsPixmapItem();

    this->selecting = false;

    this->timer = new QTimer();
    this->timer->setInterval(1000);
    this->timer->setSingleShot(true);
    this->connect(this->timer, SIGNAL(timeout()), this, SLOT(updateItemBasedMask()));

    this->isUpToDate = false;

    this->resize(1000, 768);
    this->rasterMask = new Mat();

    this->colorTable = new QVector<QRgb>();
    this->colorTable->append(0x00000000);
    this->colorTable->append(0xFFFF0000);

    this->inactiveColorTable = new QVector<QRgb>();
    this->inactiveColorTable->append(0x00000000);
    this->inactiveColorTable->append(0xA0FFFFFF);

    this->itemColorTable = new QVector<QRgb>();
    this->itemColorTable->append(0x00000000);
    this->itemColorTable->append(0xFFFF0000);

    this->selectionColorTable = new QVector<QRgb>();
    this->selectionColorTable->append(0xA0808080);
    this->selectionColorTable->append(0x00000000);

    this->picking = false;



this->openNewImage(); // debug
}

MainWidget::~MainWidget()
{

}

void MainWidget::openNewImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         "Open Image", "/home/misha/", "Image Files (*.png *.jpg)");
    if (fileName != "")
    {
        this->image = new Image((string)(fileName.toLocal8Bit()));

        for (int i = 0; i < this->image->nchannels; i++)
        {
            RangeControl* rc = new RangeControl(image->channelsNames[i].c_str(), &(image->channelsMaskLUT[i]), i);
            this->connect(rc, SIGNAL(rangeChanged()), this, SLOT(updateRasterMask()));
            this->colorRangeLayout->addWidget(rc);
            this->rangeControls.append(rc);
        }

        this->updateGraphicsViewFromMat();
    }
    ifstream fmisc((this->image->imageFileName + ".misc").c_str());
    if (fmisc)
    {
        char selecting[255];
        char sel[255];
        char selectionBrushSize[255];
        char min[255];
        char max[255];
        fmisc.getline(selecting, 255);
        fmisc.getline(sel, 255);
        fmisc.getline(selectionBrushSize, 255);
        fmisc.getline(min, 255);
        fmisc.getline(max, 255);

        if ((bool)atoi(selecting))
            this->selectionButtonClicked();
        if (!(bool)atoi(sel))
            this->invertselpainting();
        this->selectionBrushSize->setValue(atoi(selectionBrushSize));
        this->min->setValue(atoi(min));
        this->max->setValue(atoi(max));

        fmisc.close();
    }
    else qApp->exit(0);
}

void MainWidget::updateGraphicsViewFromMat(int index, bool inactive)
{
    if (index == 0)
    {
        this->originalImageItem->~QGraphicsPixmapItem();
        this->originalImageItem = this->graphicsScene->addPixmap(QPixmap::fromImage(QImage(this->image->originalImage.ptr(), this->image->originalImage.cols, this->image->originalImage.rows, this->image->originalImage.cols * this->image->originalImage.channels(),QImage::Format_RGB888)));

        this->graphicsView->show();
        this->switchTab(this->tabBar->currentIndex());
        this->updateRasterMask();
        this->updateSelectionMask();
    }
    if (index == 1)
    {
        this->switchTab(this->tabBar->currentIndex());
    }
    if (index == 2)
    {
        QGraphicsPixmapItem* old = this->maskItem;

        QImage* mask = new QImage(this->rasterMask->ptr(), this->rasterMask->cols, this->rasterMask->rows, this->rasterMask->cols * this->rasterMask->channels(), QImage::Format_Indexed8);
        if (inactive)
            mask->setColorTable(*this->inactiveColorTable);
        else
            mask->setColorTable(*this->colorTable);

        this->maskItem = this->graphicsScene->addPixmap(QPixmap::fromImage(*mask));
        this->maskItem->setZValue(1);

        mask->~QImage();

        old->~QGraphicsPixmapItem();
        this->switchTab(1);
    }
}

void MainWidget::updateSelectionMask()
{
    QGraphicsPixmapItem* oldselmask = this->selMaskItem;

    QImage* selmask = new QImage(this->image->selMask.ptr(), this->image->selMask.cols, this->image->selMask.rows, this->image->selMask.cols * this->image->selMask.channels(), QImage::Format_Indexed8);
    selmask->setColorTable(*this->selectionColorTable);
    this->selMaskItem = this->graphicsScene->addPixmap(QPixmap::fromImage(*selmask));
    this->selMaskItem->setZValue(100000000);

    oldselmask->~QGraphicsPixmapItem();
}

void MainWidget::clearItems()
{
    while (this->maskItems.size() != 0)
    {
        QGraphicsPixmapItem* subj = this->maskItems.at(this->maskItems.size() - 1);
        subj->~QGraphicsPixmapItem();
        this->maskItems.pop_back();
    }

    this->calculateButton->setText(QString::fromUtf8("ПЫЩЬ"));
}

void MainWidget::updateRasterMask()
{
    this->clearItems();
    this->rasterMask->~Mat();
    this->rasterMask = this->image->getRasterMergedMask();

    this->updateGraphicsViewFromMat(2);

    if (this->timer->isActive())
        this->timer->stop();
    this->timer->start();

}

void MainWidget::updateItemBasedMask()
{
    qDebug() << "timeout";
    //this->calculate();

}

void MainWidget::switchTab(int i)
{
    if (i == 1)
    {
        this->maskItem->show();
        //this->originalImageItem->hide();
    }
    if (i == 0)
    {
        this->maskItem->hide();
        this->originalImageItem->show();
    }
    if (i == 2)
    {

    }
}

void MainWidget::calculate()
{
    if (this->maskItems.size() == 0)
    {
        std::vector<size_t>* edges = new std::vector<size_t>();
        std::vector<cv::Point>* firstPoints = new std::vector<cv::Point>();
        std::vector<cv::Rect>* points = new std::vector<cv::Rect>();

        double t = (double)getTickCount();                      // time counting

        findColorAreas(this->image->getRasterMergedMask(), edges, firstPoints, true, points);

        cout << "Times passed in seconds to calculate: " << ((double)getTickCount() - t)/getTickFrequency() << endl;       // time counting
        t = (double)getTickCount();

        qDebug() << points->size();

        for (int i = 0; i < edges->size(); i++)
        {
            Mat *m = new Mat(*(this->rasterMask), (points->at(i)));
            Mat gitem = m->clone();
            m->~Mat();

            QImage* iitem = new QImage(gitem.ptr(), gitem.cols, gitem.rows, gitem.cols * gitem.channels(), QImage::Format_Indexed8);
            iitem->setColorTable(*this->itemColorTable);

            QGraphicsPixmapItem* subj = this->graphicsScene->addPixmap(QPixmap::fromImage(*iitem));
            subj->setPos(points->at(i).x, points->at(i).y);

            subj->setToolTip(QString::number(edges->at(i)));
            subj->setZValue(2);

            this->maskItems.push_back(subj);
        }

        this->updateGraphicsViewFromMat(2, true);

        this->edges = edges;

        this->rangeChanged();

        cout << "Times passed in seconds to view numbers: " << ((double)getTickCount() - t)/getTickFrequency() << endl;       // time counting

        this->calculateButton->setText(QString::fromUtf8("Show results"));
    }
    else
    {
        std::vector<size_t>* edges = new std::vector<size_t>();
        for (int i = 0; i < this->edges->size(); i++)
        {
            if (this->max->value() >= 0)
            {
                if ((this->edges->at(i) > this->min->value()) && (this->edges->at(i) < this->max->value()))
                        edges->push_back(this->edges->at(i));
            }
            else
            {
                if ((this->edges->at(i) > this->min->value()))
                        edges->push_back(this->edges->at(i));
            }
        }
        ResultInfo* info = new ResultInfo(edges, QString::fromStdString(this->image->imageFileName), this->image);
    }
}

void MainWidget::selectionButtonClicked()
{
    if (this->selecting)
    {
        this->disconnect(this->graphicsView, SIGNAL(mouseMoved(QPointF)), this, SLOT(newPoint(QPointF)));
        this->selectionButton->setText(QString::fromUtf8("Paint mask"));
    }
    else
    {
        this->connect(this->graphicsView, SIGNAL(mouseMoved(QPointF)), this, SLOT(newPoint(QPointF)));
        this->selectionButton->setText(QString::fromUtf8("Stop painting mask"));
    }

    this->selecting = !(this->selecting);
}

void MainWidget::newPoint(QPointF point)
{
    circle(this->image->selMask, Point(point.rx(), point.ry()), this->selectionBrushSize->value(), this->sel, -1, 8);
    this->updateSelectionMask();
    this->updateRasterMask();
}

void MainWidget::pickStart()
{
    if (this->picking)
    {
        this->disconnect(this->graphicsView, SIGNAL(mousePressed(QPointF)), this, SLOT(pick(QPointF)));
        this->pickButton->setText(QString::fromUtf8("Pick"));
    }
    else
    {
        this->connect(this->graphicsView, SIGNAL(mousePressed(QPointF)), this, SLOT(pick(QPointF)));
        this->pickButton->setText(QString::fromUtf8("Picking..."));
    }
    this->picking = !(this->picking);
}

void MainWidget::pick(QPointF point)
{    
    for (int i = 0; i < this->image->nchannels; i++)
    {
        int color = this->image->channels[i].at<uchar>(point.ry(), point.rx());
        int l;
        int h;
        if (color - colordelta >= 0)
            l = color - colordelta;
        else
            l = 0;

        if (color + colordelta <= 255)
            h = color + colordelta;
        else
            h = 255;

        this->rangeControls.at(i)->setRange(l, h);
    }

    this->pickStart();
}

void MainWidget::selectionOnesClicked()
{
}

void MainWidget::invertselpainting()
{
    if ((bool)this->sel)
    {
        this->positiveNegativePainting->setText(QString::fromUtf8("Set positive"));
        this->sel = (uchar)0;
    }
    else
    {
        this->positiveNegativePainting->setText(QString::fromUtf8("Set negative"));
        this->sel = (uchar)1;
    }
}

void MainWidget::rangeChanged()
{
    for (int i = 0; i < this->maskItems.size(); i++)
    {
        if (this->max->value() >= 0)
        {
            if ((this->edges->at(i) > this->min->value()) && (this->edges->at(i) < this->max->value()))
                    this->maskItems.at(i)->setVisible(true);
            else
                this->maskItems.at(i)->setVisible(false);
        }
        else
        {
            if ((this->edges->at(i) > this->min->value()))
                    this->maskItems.at(i)->setVisible(true);
            else
                this->maskItems.at(i)->setVisible(false);
        }
    }
}

void MainWidget::saveSelection()
{
    this->image->saveSel();
}

void MainWidget::saveMisc()
{
    ofstream fmisc((this->image->imageFileName + ".misc").c_str());
    fmisc << (int)(this->selecting) << std::endl;
    fmisc << (int)(this->sel) << std::endl;
    fmisc << this->selectionBrushSize->value() << std::endl;
    fmisc << this->min->value() << std::endl;
    fmisc << this->max->value() << std::endl;
    fmisc.flush();
    fmisc.close();
}

void MainWidget::saveColorRange()
{
    this->image->saveChannelMask();
}

void MainWidget::saveAll()
{
    this->saveSelection();
    this->saveMisc();
    this->saveColorRange();
    this->saveGeometry();
}
