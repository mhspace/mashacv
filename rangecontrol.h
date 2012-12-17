#ifndef RANGECONTROL_H
#define RANGECONTROL_H

#include <QWidget>
#include <QPainter>

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
    void setMax(double max);
    void setMin(double min);
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
    
private slots:
    void on_lowerRangeLimit_valueChanged(double arg1);
    void on_upperRangeLimit_valueChanged(double arg1);

private:
    Ui::RangeControl *ui;
    bool allowOverlap;
    QBrush backgroundBrush;
};

#endif // RANGECONTROL_H
