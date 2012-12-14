#include "toolsdockwidget.h"
#include "ui_toolsdockwidget.h"
#include <QDebug>
ToolsDockWidget::ToolsDockWidget(unsigned int maxAreaSize, int maxBrushSize, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ToolsDockWidget)
{
    ui->setupUi(this);
    qDebug() << maxBrushSize << "brush";
    ui->maskBrushSizeSlider->setMinimum(0);
    ui->maskBrushSizeSpinBox->setMinimum(0);
    ui->maskBrushSizeSlider->setMaximum(maxBrushSize);
    ui->maskBrushSizeSpinBox->setMaximum(maxBrushSize);

    ui->maxSizeSlider->setMaximum(maxAreaSize);
    ui->maxSizeSpinBox->setMaximum(maxAreaSize);

    ui->minSizeSlider->setMaximum(maxAreaSize);
    ui->minSizeSpinBox->setMaximum(maxAreaSize);

    this->setMaxSize(maxAreaSize);
}

ToolsDockWidget::~ToolsDockWidget()
{
    delete ui;
}

int ToolsDockWidget::maskBrushSize()
{
    return ui->maskBrushSizeSpinBox->value();
}

int ToolsDockWidget::maxSize()
{
    return ui->maxSizeSlider->value();
}

void ToolsDockWidget::setMaxSize(int value)
{
    ui->maxSizeSlider->setValue(value);
}

int ToolsDockWidget::minSize()
{
    return ui->minSizeSlider->value();
}

void ToolsDockWidget::setMinSize(int value)
{
    ui->minSizeSlider->setValue(value);
}

bool ToolsDockWidget::isSizeRangeEnabled()
{
    return ui->groupBox_2->isChecked();
}

void ToolsDockWidget::setSizeRangeEnabled(bool val)
{
    ui->groupBox_2->setChecked(val);
}

bool ToolsDockWidget::isDrawingMask()
{
    return ui->drawMaskRadioButton->isChecked();
}

bool ToolsDockWidget::isErasingMask()
{
    return ui->eraseMaskRadioButton->isChecked();
}

void ToolsDockWidget::changeEvent(QEvent *e)
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


void ToolsDockWidget::on_drawMaskRadioButton_clicked()
{
    emit drawMask();
}

void ToolsDockWidget::on_eraseMaskRadioButton_clicked()
{
    emit eraseMask();
}

void ToolsDockWidget::on_chooseColorRadioButton_clicked()
{
    emit pickColor();
}

void ToolsDockWidget::on_moveRadioButton_clicked()
{
    emit move();
}

void ToolsDockWidget::on_maskBrushSizeSlider_valueChanged(int value)
{
    this->ui->maskBrushSizeSpinBox->setValue(value);
}

void ToolsDockWidget::on_maskBrushSizeSpinBox_valueChanged(int arg1)
{
    this->ui->maskBrushSizeSlider->setValue(arg1);
    emit maskBrushSizeChanged();
}

void ToolsDockWidget::on_completelyEraseMaskButton_clicked()
{
    emit completelyEraseMask();
}

void ToolsDockWidget::on_completelyFillMaskButton_clicked()
{
    emit completelyFillMask();
}

void ToolsDockWidget::on_minSizeSpinBox_valueChanged(int arg1)
{
    ui->minSizeSlider->setValue(arg1);
    emit minSizeChanged();
    emit sizeRangeChanged();
}

void ToolsDockWidget::on_minSizeSlider_valueChanged(int value)
{
    ui->minSizeSpinBox->setValue(value);
    emit minSizeChanged();
    emit sizeRangeChanged();
}

void ToolsDockWidget::on_maxSizeSpinBox_valueChanged(int arg1)
{
    ui->maxSizeSlider->setValue(arg1);
    emit maxSizeChanged();
    emit sizeRangeChanged();
}

void ToolsDockWidget::on_maxSizeSlider_valueChanged(int value)
{
    ui->maxSizeSpinBox->setValue(value);
    emit maxSizeChanged();
    emit sizeRangeChanged();
}

void ToolsDockWidget::on_groupBox_2_toggled(bool arg1)
{
    ui->minLabel->setEnabled(arg1);
    ui->minSizeSlider->setEnabled(arg1);
    ui->minSizeSpinBox->setEnabled(arg1);
    ui->maxLabel->setEnabled(arg1);
    ui->maxSizeSlider->setEnabled(arg1);
    ui->maxSizeSpinBox->setEnabled(arg1);
    emit sizeRangeEnabledChanged();
}
