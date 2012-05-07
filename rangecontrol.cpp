#include "rangecontrol.h"

RangeControl::RangeControl(QString channelName, Mat *channelMaskLUT, unsigned int channelIndex, QWidget *parent) :
    QGroupBox(channelName, parent)
{
    this->upperLimitOfTheRange = new QSlider(Qt::Horizontal);


    if (channelName == "H")
    {
        this->huepixmap = new QPixmap(QString::fromUtf8(":/images/hueline.png"));
        this->huelabel = new QLabel();
        this->huelabel->setPixmap(*this->huepixmap);
        this->huelabel->setScaledContents(true);
    }

    this->lowerLimitOfTheRange = new QSlider(Qt::Horizontal);


    this->channelName = channelName;
    this->channelMaskLUT = channelMaskLUT;

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(this->lowerLimitOfTheRange);
    if (channelName == "H")
        layout->addWidget(this->huelabel);
    layout->addWidget(this->upperLimitOfTheRange);

    this->lowerLimitOfTheRange->setMinimum(0);
    this->lowerLimitOfTheRange->setSingleStep(1);
    this->lowerLimitOfTheRange->setMaximum(255);

    this->upperLimitOfTheRange->setMinimum(0);
    this->upperLimitOfTheRange->setSingleStep(1);
    this->upperLimitOfTheRange->setMaximum(255);

    uchar* p = (*(this->channelMaskLUT)).data;

    bool oneFinded = false;

    for (int i = 0; i < 256; i++)
        if (!oneFinded)
        {
            if (p[i] == (uchar)1)
            {
                this->lowerLimitOfTheRange->setSliderPosition(i);
                oneFinded = true;
            }
        }
        else
            if (p[i] == (uchar)0)
            {
                this->upperLimitOfTheRange->setSliderPosition(i);
                break;
            }
            else
                if ((p[i] == (uchar)1))
                    this->upperLimitOfTheRange->setSliderPosition(i);


    this->connect(this->lowerLimitOfTheRange, SIGNAL(valueChanged(int)), this, SLOT(updateChannelMaskLUT()));
    this->connect(this->upperLimitOfTheRange, SIGNAL(valueChanged(int)), this, SLOT(updateChannelMaskLUT()));
    this->setLayout(layout);
}

void RangeControl::setRange(int l, int u)
{
    this->upperLimitOfTheRange->setValue(u);
    this->lowerLimitOfTheRange->setValue(l);
}

void RangeControl::updateChannelMaskLUT()
{
    uchar* p = (*(this->channelMaskLUT)).data;
    for (int i = 0; i < 256; i++)
        if ((this->lowerLimitOfTheRange->value() <= i) and (this->upperLimitOfTheRange->value() >= i))
            p[i] = (uchar)1;
        else
            p[i] = (uchar)0;
    emit rangeChanged();
}
