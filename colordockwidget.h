#ifndef COLORDOCKWIDGET_H
#define COLORDOCKWIDGET_H

#include <QDockWidget>
#include <QColor>
#include <QRgb>
#include <QColorDialog>

namespace Ui {
class ColorDockWidget;
}

class ColorDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ColorDockWidget(QWidget *parent = 0);
    ~ColorDockWidget();
    void setColor(QColor colorPrivate);
    QColor *getColor();
    double getDelta();
    QRgb rgb();

protected:
    void changeEvent(QEvent *e);
    
private slots:
    void on_colorButton_clicked();

    void on_thresholdDoubleSpinBox_valueChanged(double arg1);

    void on_thresholdSlider_valueChanged(int value);
private:
    Ui::ColorDockWidget *ui;
    QColor *colorPrivate;
    void deltaEChange(double deltaE);
signals:
    void colorOrDeltaChanged();
};

#endif // COLORDOCKWIDGET_H
