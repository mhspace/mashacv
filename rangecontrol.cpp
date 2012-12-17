#include "rangecontrol.h"
#include "ui_rangecontrol.h"

RangeControl::RangeControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RangeControl)
{
    ui->setupUi(this);
    this->setMin(0);
    this->setMax(255);
    this->allowOverlap = false;
    ui->label->setText("asdf");//label;
}



RangeControl::~RangeControl()
{
    delete ui;
}

double RangeControl::lowerRangeLimit()
{
    return ui->lowerRangeLimit->value();
}

double RangeControl::upperRangeLimit()
{
    return ui->upperRangeLimit->value();
}

void RangeControl::setMax(double max)
{
    ui->upperRangeLimit->setMaximum(max);
    ui->lowerRangeLimit->setMaximum(max);
    this->repaint();
}

void RangeControl::setMin(double min)
{
    ui->upperRangeLimit->setMinimum(min);
    ui->lowerRangeLimit->setMinimum(min);
    this->repaint();
}

double RangeControl::min()
{
    return ui->upperRangeLimit->minimum();
}

double RangeControl::max()
{
    return ui->lowerRangeLimit->maximum();
}

void RangeControl::setLabel(QString label)
{
    ui->label->setText("<pre>"+label+"</pre>");
    this->repaint();
}

void RangeControl::setAllowOverlap(bool allowOverlap)
{
    this->allowOverlap = allowOverlap;
    this->repaint();
}

void RangeControl::setBackground(QBrush brush)
{

    this->repaint();
}

void RangeControl::changeEvent(QEvent *e)
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

void RangeControl::paintEvent(QPaintEvent *paintEvent)
{
    QPainter painter(this);

    double xMult = ui->previewLabel->width()/max();
    painter.setBrush(qApp->palette().background());
    painter.drawRect(ui->previewLabel->x(), ui->upperRangeLimit->y(), ui->previewLabel->width(), ui->upperRangeLimit->height());

    if (lowerRangeLimit() > upperRangeLimit())
    {
        if (allowOverlap)
        {
            painter.setBrush(qApp->palette().highlight());

            painter.drawRect(ui->previewLabel->x(), ui->lowerRangeLimit->y(), upperRangeLimit()*xMult , ui->lowerRangeLimit->height());
            painter.drawRect(ui->previewLabel->x() + lowerRangeLimit()*xMult, ui->lowerRangeLimit->y(), ui->previewLabel->width() - lowerRangeLimit()*xMult, ui->lowerRangeLimit->height());
        }
        else
        {
            painter.setBrush(Qt::red);
            painter.drawRect(ui->previewLabel->x() + (lowerRangeLimit()*xMult), ui->lowerRangeLimit->y(), upperRangeLimit()*xMult - (lowerRangeLimit()*xMult), ui->lowerRangeLimit->height());
        }
    }
    else
    {
        painter.setBrush(qApp->palette().highlight());
        painter.drawRect(ui->previewLabel->x() + (lowerRangeLimit()*xMult), ui->lowerRangeLimit->y(), upperRangeLimit()*xMult - (lowerRangeLimit()*xMult), ui->lowerRangeLimit->height());
    }



}

void RangeControl::on_lowerRangeLimit_valueChanged(double arg1)
{
    emit rangeChanged();
    this->repaint();
}

void RangeControl::on_upperRangeLimit_valueChanged(double arg1)
{
    emit rangeChanged();
    this->repaint();
}
