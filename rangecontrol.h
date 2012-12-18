#ifndef RANGECONTROL_H
#define RANGECONTROL_H

#include <QWidget>
#include <QPainter>
#include <QDebug>

namespace Ui {
class RangeControl;
}

class RangeControl : public QWidget
{
    Q_OBJECT
    
public:
    explicit RangeControl(QWidget *parent = 0);
    ~RangeControl();
    double lowerRangeLimit();
    double upperRangeLimit();
    void setLowerRangeLimit(double val);
    void setUpperRangeLimit(double val);
    void setMax(double max);
    void setMin(double min);
    void setSingleStep(double step);
    double min();
    double max();
    void setLabel(QString label);
    void setAllowOverlap(bool allowOverlap);
    void setBackground(QBrush brush);
    
signals:
    void rangeChanged();

protected:
    void changeEvent(QEvent *e);
    void paintEvent(QPaintEvent *paintEvent);
    void mouseMoveEvent (QMouseEvent *event);
    void mousePressEvent (QMouseEvent *event);
    void mouseReleaseEvent (QMouseEvent *event);
    
private slots:
    void on_lowerRangeLimit_valueChanged(double arg1);
    void on_upperRangeLimit_valueChanged(double arg1);

private:
    enum slider{upperRangeLimitSlider, lowerRangeLimitSlider};
    Ui::RangeControl *ui;
    bool allowOverlap;
    bool isDraggingSlider;
    double lastXpos;
    slider draggingSlider;
    QBrush backgroundBrush;
};

#endif // RANGECONTROL_H
