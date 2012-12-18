#include "colordockwidget.h"
#include "ui_colordockwidget.h"

#define CIEDE2000_MAX 450.0
#define CIEDE2000_TOINTmult 100

ColorDockWidget::ColorDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ColorDockWidget)
{
    ui->setupUi(this);
    this->colorPrivate = new QColor(Qt::black);
    this->setColor(*(this->colorPrivate));

    ui->methodBox->addItem(tr("Base Color and Delta E"), QVariant((ui->stackedWidget->indexOf((ui->color_colorDelta)))));
    ui->methodBox->addItem(tr("Color channels ranges"), QVariant((ui->stackedWidget->indexOf((ui->colorModel_channelRanges)))));

    ui->colorModel_channelRanges_firstChannel->setLabel("H");
    ui->colorModel_channelRanges_firstChannel->setAllowOverlap(true);
    ui->colorModel_channelRanges_firstChannel->setMax(360);
    ui->colorModel_channelRanges_firstChannel->setSingleStep(0.1);
    QLinearGradient hueGradient(0, 0, 1, 0);
    hueGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    hueGradient.setColorAt(  0.0/360, Qt::red);
    hueGradient.setColorAt( 60.0/360, Qt::yellow);
    hueGradient.setColorAt(120.0/360, Qt::green);
    hueGradient.setColorAt(180.0/360, Qt::cyan);
    hueGradient.setColorAt(240.0/360, Qt::blue);
    hueGradient.setColorAt(300.0/360, Qt::magenta);
    hueGradient.setColorAt(360.0/360, Qt::red);
    ui->colorModel_channelRanges_firstChannel->setBackground(QBrush(hueGradient));
    ui->colorModel_channelRanges_secondChannel->setLabel("S");
    ui->colorModel_channelRanges_secondChannel->setMax(1);
    ui->colorModel_channelRanges_secondChannel->setSingleStep(0.001);
    ui->colorModel_channelRanges_thirdChannel->setLabel("V");
    ui->colorModel_channelRanges_thirdChannel->setMax(1);
    ui->colorModel_channelRanges_thirdChannel->setSingleStep(0.001);

    connect(ui->colorModel_channelRanges_firstChannel, SIGNAL(rangeChanged()), this, SIGNAL(colorOrDeltaChanged()));
    connect(ui->colorModel_channelRanges_secondChannel, SIGNAL(rangeChanged()), this, SIGNAL(colorOrDeltaChanged()));
    connect(ui->colorModel_channelRanges_thirdChannel, SIGNAL(rangeChanged()), this, SIGNAL(colorOrDeltaChanged()));


    ui->thresholdDoubleSpinBox->setMinimum(0);
    ui->thresholdDoubleSpinBox->setMaximum(CIEDE2000_MAX);
    ui->thresholdSlider->setMinimum(0);
    ui->thresholdSlider->setMaximum((int)(CIEDE2000_MAX*CIEDE2000_TOINTmult));
}

ColorDockWidget::~ColorDockWidget()
{
    delete ui;
    delete this->colorPrivate;
}


#include <QIcon>
#include <QPainter>
#include <QPixmap>
void ColorDockWidget::setColor(QColor color)
{
    (*(this->colorPrivate)) = color;
    ui->colorButton->setText(this->colorPrivate->toRgb().name());

    //refreshing color icon
    int iconsize = ui->colorButton->fontMetrics().height();
    QPixmap pixmap(iconsize*2, iconsize);
    QPainter painter(&pixmap);
    painter.fillRect(0, 0, iconsize*2, iconsize, (*(this->colorPrivate)));
    QIcon icon = ui->colorButton->icon();
    icon.addPixmap(pixmap);
    ui->colorButton->setIcon(icon);
    ui->colorButton->setIconSize(QSize(iconsize*2, iconsize));

    emit colorOrDeltaChanged();
}

QColor *ColorDockWidget::getColor()
{
    return this->colorPrivate;
}

double ColorDockWidget::getDelta()
{
    return this->ui->thresholdDoubleSpinBox->value();
}

QRgb ColorDockWidget::rgb()
{
    return this->colorPrivate->rgb();
}

int ColorDockWidget::method()
{
    return ui->stackedWidget->currentIndex();
}

float ColorDockWidget::lowerLimit(int i)
{
    if (i == 0)
        return ui->colorModel_channelRanges_firstChannel->lowerRangeLimit();
    else if (i == 1) {
        return ui->colorModel_channelRanges_secondChannel->lowerRangeLimit();
    }
    else if (i == 2) {
        return ui->colorModel_channelRanges_thirdChannel->lowerRangeLimit();
    }
}

float ColorDockWidget::upperLimit(int i)
{
    if (i == 0)
        return ui->colorModel_channelRanges_firstChannel->upperRangeLimit();
    else if (i == 1) {
        return ui->colorModel_channelRanges_secondChannel->upperRangeLimit();
    }
    else if (i == 2) {
        return ui->colorModel_channelRanges_thirdChannel->upperRangeLimit();
    }
}

void ColorDockWidget::setLowerLimit(int i, float limit)
{
    if (i == 0)
        ui->colorModel_channelRanges_firstChannel->setLowerRangeLimit(limit);
    else if (i == 1) {
        ui->colorModel_channelRanges_secondChannel->setLowerRangeLimit(limit);
    }
    else if (i == 2) {
        ui->colorModel_channelRanges_thirdChannel->setLowerRangeLimit(limit);
    }
}

void ColorDockWidget::setUpperLimit(int i, float limit)
{
    if (i == 0)
        ui->colorModel_channelRanges_firstChannel->setUpperRangeLimit(limit);
    else if (i == 1) {
        ui->colorModel_channelRanges_secondChannel->setUpperRangeLimit(limit);
    }
    else if (i == 2) {
        ui->colorModel_channelRanges_thirdChannel->setUpperRangeLimit(limit);
    }
}



void ColorDockWidget::changeEvent(QEvent *e)
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

void ColorDockWidget::on_colorButton_clicked()
{
    //TODO: предусмотреть вариант Cancel у диалога.
    this->setColor(QColorDialog::getColor(*this->colorPrivate, this, tr("Select Color")));
}

void ColorDockWidget::on_thresholdDoubleSpinBox_valueChanged(double arg1)
{
    this->deltaEChange(arg1);
}

void ColorDockWidget::on_thresholdSlider_valueChanged(int value)
{
    this->deltaEChange((double)value/(double)CIEDE2000_TOINTmult);
}

void ColorDockWidget::deltaEChange(double deltaE)
{
    //TODO: изучить странный баг: slider застревает при увеличении (только ли?) стрелками клавиатуры. При уменьшении бывают проскальзывания.
    ui->thresholdDoubleSpinBox->setValue(deltaE);
    ui->thresholdSlider->setValue((int)(deltaE*CIEDE2000_TOINTmult));

    emit colorOrDeltaChanged();
}

void ColorDockWidget::on_methodBox_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(ui->methodBox->itemData(index).toInt());
    emit colorOrDeltaChanged();
    if (index == 0)
    {
        ui->colorModel_channelRanges->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        ui->color_colorDelta->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }
    else if (index == 1) {
        ui->color_colorDelta->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        ui->colorModel_channelRanges->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }
}
