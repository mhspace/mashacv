#ifndef TOOLSDOCKWIDGET_H
#define TOOLSDOCKWIDGET_H

#include <QDockWidget>
#include <QMenu>
#include <QDir>
#include <QFileInfo>

namespace Ui {
class ToolsDockWidget;
}

class ToolsDockWidget : public QDockWidget
{
    Q_OBJECT

    Q_PROPERTY(int maxSize
               READ maxSize
               WRITE setMaxSize
               NOTIFY maxSizeChanged)
    Q_PROPERTY(int minSize
               READ minSize
               WRITE setMinSize
               NOTIFY minSizeChanged)
    Q_PROPERTY(bool sizeRangeEnabled
               READ isSizeRangeEnabled
               WRITE setSizeRangeEnabled
               NOTIFY sizeRangeEnabledChanged)
    
public:
    explicit ToolsDockWidget(unsigned int maxAreaSize = 1000, int maxBrushSize = 500, QString imageFileStrPath = QString(), QWidget *parent = 0);
    ~ToolsDockWidget();
    int maskBrushSize();
    int maxSize();
    void setMaxSize(int value);
    int minSize();
    void setMinSize(int value);
    bool isSizeRangeEnabled();
    void setSizeRangeEnabled(bool val);
    bool isDrawingMask();
    bool isErasingMask();

    QVariant dataForSave();
    void fromData(QVariant data);
    
signals:
    void move();
    void drawMask();
    void eraseMask();
    void pickColor();
    void completelyEraseMask();
    void completelyFillMask();
    void maskBrushSizeChanged();
    void minSizeChanged();
    void maxSizeChanged();
    void sizeRangeChanged();
    void sizeRangeEnabledChanged();
    void maskSaveMenuTriggered(QString filename);
    void maskLoadMenuTriggered(QString filename);
    void otherSaveMenuTriggered(QString filename);
    void otherLoadMenuTriggered(QString filename);

protected:
    void changeEvent(QEvent *e);
    
private slots:
    void on_drawMaskRadioButton_clicked();
    void on_eraseMaskRadioButton_clicked();
    void on_chooseColorRadioButton_clicked();
    void on_moveRadioButton_clicked();
    void on_maskBrushSizeSlider_valueChanged(int value);
    void on_maskBrushSizeSpinBox_valueChanged(int arg1);
    void on_completelyEraseMaskButton_clicked();
    void on_completelyFillMaskButton_clicked();
    void on_minSizeSpinBox_valueChanged(int arg1);
    void on_minSizeSlider_valueChanged(int value);
    void on_maxSizeSpinBox_valueChanged(int arg1);
    void on_maxSizeSlider_valueChanged(int value);
    void on_groupBox_2_toggled(bool arg1);
    void on_maskSaveMenu_triggered(QAction* action);
    void on_maskLoadMenu_triggered(QAction* action);
    void on_otherSaveMenu_triggered(QAction* action);
    void on_otherLoadMenu_triggered(QAction* action);

    void on_maskLoadButton_clicked();

    void on_otherLoadButton_clicked();

private:
    Ui::ToolsDockWidget *ui;
    QString imageFileStrPath;
    QMenu maskSaveMenu;
    QMenu maskLoadMenu;
    QMenu otherSaveMenu;
    QMenu otherLoadMenu;
    QFileInfo* imageFile;
    QDir* imageDir;
};

#endif // TOOLSDOCKWIDGET_H
