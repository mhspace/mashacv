#ifndef RANGECONTROL_H
#define RANGECONTROL_H

#include <QGroupBox>

#include "opencv2/core/core.hpp"
#include <opencv2/opencv.hpp>

#include <QtGui/QSlider>
#include <QVBoxLayout>

#include <QDebug>

using namespace cv;

class RangeControl : public QGroupBox
{
    Q_OBJECT
public:
    explicit RangeControl(QString channelName, Mat* channelMaskLUT, unsigned int channelIndex, QWidget *parent = 0);
    unsigned int channelIndex;
    void setRange(int l, int u);
    QString channelName;
    Mat* channelMaskLUT;


signals:
    void rangeChanged();
    
public slots:

private:
    QSlider* lowerLimitOfTheRange;
    QSlider* upperLimitOfTheRange;

private slots:
    void updateChannelMaskLUT();
    
};

#endif // RANGECONTROL_H
