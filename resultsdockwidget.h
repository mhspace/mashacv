#ifndef RESULTSDOCKWIDGET_H
#define RESULTSDOCKWIDGET_H

#include <QDockWidget>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

namespace Ui {
class ResultsDockWidget;
}

class ResultsDockWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ResultsDockWidget(QString fileName, QWidget *parent = 0);
    ~ResultsDockWidget();
    void disableResults();
    void showResults(QString results);
    void showFilteredResults(QString results);
    
protected:
    void changeEvent(QEvent *e);
    
public slots:
    void on_pushButton_clicked();

private:
    Ui::ResultsDockWidget *ui;
    QString results;
    QString filteredResults;
    QString fileName;
};

#endif // RESULTSDOCKWIDGET_H
