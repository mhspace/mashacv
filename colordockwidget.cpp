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
