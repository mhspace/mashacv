#include "toolsdockwidget.h"
#include "ui_toolsdockwidget.h"
#include <QDebug>
#include <QMenu>
#include <QVariant>
ToolsDockWidget::ToolsDockWidget(unsigned int maxAreaSize, int maxBrushSize, QString imageFileStrPath, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ToolsDockWidget)
{
    ui->setupUi(this);
    this->imageFileStrPath = imageFileStrPath;
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

    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Preferred);
    policy.setVerticalPolicy(QSizePolicy::Fixed);

    this->widget()->setSizePolicy(policy);
    ui->maskSaveButton->setMenu(&maskSaveMenu);
    ui->maskLoadButton->setMenu(&maskLoadMenu);
    ui->otherLoadButton->setMenu(&otherLoadMenu);
    ui->otherSaveButton->setMenu(&otherSaveMenu);

    connect(&maskSaveMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_maskSaveMenu_triggered(QAction*)));
    connect(&maskLoadMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_maskLoadMenu_triggered(QAction*)));
    connect(&otherSaveMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_otherSaveMenu_triggered(QAction*)));
    connect(&otherLoadMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_otherLoadMenu_triggered(QAction*)));

    imageFile = new QFileInfo(imageFileStrPath);
    imageDir = new QDir(imageFile->absoluteDir());

    QAction* maskSaveForThis = new QAction(QString("Save for this image"), this);
    maskSaveForThis->setData(imageFile->absoluteFilePath() + ".mask.png");
    maskSaveForThis->setStatusTip(imageFile->absoluteFilePath() + ".mask.png");
    maskSaveMenu.addAction(maskSaveForThis);
    QAction* maskSaveForThisDir = new QAction(QString("Save for this dir (" + imageDir->dirName()+")"), this);
    maskSaveForThisDir->setData(imageDir->absoluteFilePath("mask.png"));
    maskSaveForThisDir->setStatusTip(imageDir->absoluteFilePath("mask.png"));
    maskSaveMenu.addAction(maskSaveForThisDir);
    QDir* imgDir = new QDir(*imageDir);
    while (imgDir->cdUp())
    {
        QAction* saveForDir = new QAction(QString("Save for \"" + imgDir->dirName()+"\""), this);
        saveForDir->setData(imgDir->absoluteFilePath("mask.png"));
        saveForDir->setStatusTip(imgDir->absoluteFilePath("mask.png"));

        maskSaveMenu.addAction(saveForDir);
    }
    imgDir->~QDir();
///////////////
    QAction* otherSaveForThis = new QAction(QString("Save for this image"), this);
    otherSaveForThis->setData(imageFile->absoluteFilePath() + ".MashaCV.conf");
    otherSaveForThis->setStatusTip(imageFile->absoluteFilePath() + ".MashaCV.conf");
    otherSaveMenu.addAction(otherSaveForThis);
    QAction* otherSaveForThisDir = new QAction(QString("Save for this dir (" + imageDir->dirName()+")"), this);
    otherSaveForThisDir->setData(imageDir->absoluteFilePath("MashaCV.conf"));
    otherSaveForThisDir->setStatusTip(imageDir->absoluteFilePath("MashaCV.conf"));
    otherSaveMenu.addAction(otherSaveForThisDir);
    imgDir = new QDir(*imageDir);
    while (imgDir->cdUp())
    {
        QAction* saveForDir = new QAction(QString("Save for \"" + imgDir->dirName()+"\""), this);
        saveForDir->setData(imgDir->absoluteFilePath("MashaCV.conf"));
        saveForDir->setStatusTip(imgDir->absoluteFilePath("MashaCV.conf"));

        otherSaveMenu.addAction(saveForDir);
    }
    imgDir->~QDir();
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

QVariant ToolsDockWidget::dataForSave()
{
    QMap<QString, QVariant> data;
    data["isSizeRangeEnabled"] = isSizeRangeEnabled();
    data["lowerRangeLimit"] = minSize();
    data["upperRangeLimit"] = maxSize();
    return data;
}

void ToolsDockWidget::fromData(QVariant data)
{
    QMap<QString, QVariant> mapData = data.toMap();
    setSizeRangeEnabled(mapData["isSizeRangeEnabled"].toBool());
    setMinSize(mapData["lowerRangeLimit"].toInt());
    setMaxSize(mapData["upperRangeLimit"].toInt());
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

void ToolsDockWidget::on_maskSaveMenu_triggered(QAction *action)
{
    qDebug() << "on_maskSaveMenu_triggered" << action->text() << action->data();
    emit maskSaveMenuTriggered(action->data().toString());
}

void ToolsDockWidget::on_maskLoadMenu_triggered(QAction *action)
{
    if (action->data().isValid())
    {
        emit maskLoadMenuTriggered(action->data().toString());
    }
    qDebug() << "on_maskLoadMenu_triggered" << action->text() << action->data();
}

void ToolsDockWidget::on_otherSaveMenu_triggered(QAction *action)
{
    emit otherSaveMenuTriggered(action->data().toString());
    qDebug() << "on_otherSaveMenu_triggered" << action->text() << action->data();
}

void ToolsDockWidget::on_otherLoadMenu_triggered(QAction *action)
{
    qDebug() << "on_otherLoadMenu_triggered" << action->text() << action->data();
    if (action->data().isValid())
    {
        emit otherLoadMenuTriggered(action->data().toString());
    }
}

void ToolsDockWidget::on_maskLoadButton_clicked()
{
    maskLoadMenu.clear();

    int maskCount = 0;

    QFileInfo maskForImageFileInfo(imageFileStrPath + ".mask.png");
    if (maskForImageFileInfo.exists())
    {
        QAction* act = new QAction(QString("Load mask for this image"), this);
        act->setData(imageFileStrPath + ".mask.png");
        act->setStatusTip(imageFileStrPath + ".mask.png");
        maskLoadMenu.addAction(act);
        maskCount++;
    }

    QFileInfo asdsd(imageDir->absoluteFilePath("mask.png"));
    if (asdsd.exists())
    {
        QAction* act = new QAction(QString("Load mask for this dir (" + imageDir->dirName()+")"), this);
        act->setData(imageDir->absoluteFilePath("mask.png"));
        act->setStatusTip(imageDir->absoluteFilePath("mask.png"));
        maskLoadMenu.addAction(act);
        maskCount++;
    }

    QDir* imgDir = new QDir(*imageDir);
    while (imgDir->cdUp())
    {
        QFileInfo fileInfo(imgDir->absoluteFilePath("mask.png"));
        if (fileInfo.exists())
        {
            QAction* asdf = new QAction(QString("Load for \"" + imgDir->dirName()+"\""), this);
            asdf->setData(imgDir->absoluteFilePath("mask.png"));
            asdf->setStatusTip(imgDir->absoluteFilePath("mask.png"));

            maskLoadMenu.addAction(asdf);
            maskCount++;
        }
    }
    imgDir->~QDir();

    if (maskCount == 0)
        maskLoadMenu.addAction(tr("No saved masks found"));

    ui->maskLoadButton->showMenu();
}

void ToolsDockWidget::on_otherLoadButton_clicked()
{
    otherLoadMenu.clear();

    int otherConfsCount = 0;

    QFileInfo configFileInfo(imageFileStrPath + ".MashaCV.conf");
    if (configFileInfo.exists())
    {
        QAction* act = new QAction(QString("Load config for this image"), this);
        act->setData(imageFileStrPath + ".MashaCV.conf");
        act->setStatusTip(imageFileStrPath + ".MashaCV.conf");
        otherLoadMenu.addAction(act);
        otherConfsCount++;
    }

    QFileInfo asdsd(imageDir->absoluteFilePath("MashaCV.conf"));
    if (asdsd.exists())
    {
        QAction* act = new QAction(QString("Load config for this dir (" + imageDir->dirName()+")"), this);
        act->setData(imageDir->absoluteFilePath("MashaCV.conf"));
        act->setStatusTip(imageDir->absoluteFilePath("MashaCV.conf"));
        otherLoadMenu.addAction(act);
        otherConfsCount++;
    }

    QDir* imgDir = new QDir(*imageDir);
    while (imgDir->cdUp())
    {
        QFileInfo fileInfo(imgDir->absoluteFilePath("MashaCV.conf"));
        if (fileInfo.exists())
        {
            QAction* asdf = new QAction(QString("Load for \"" + imgDir->dirName()+"\""), this);
            asdf->setData(imgDir->absoluteFilePath("MashaCV.conf"));
            asdf->setStatusTip(imgDir->absoluteFilePath("MashaCV.conf"));

            otherLoadMenu.addAction(asdf);
            otherConfsCount++;
        }
    }
    imgDir->~QDir();

    if (otherConfsCount == 0)
        otherLoadMenu.addAction(tr("No saved configs found"));

    ui->otherLoadButton->showMenu();
}
