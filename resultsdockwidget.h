#ifndef RESULTSDOCKWIDGET_H
#define RESULTSDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class ResultsDockWidget;
}

class ResultsDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ResultsDockWidget(QWidget *parent = 0);
    ~ResultsDockWidget();
    void disableResults();
    void showResults(QString results);
    void showFilteredResults(QString results);
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::ResultsDockWidget *ui;
    QString results;
    QString filteredResults;
};

#endif // RESULTSDOCKWIDGET_H
