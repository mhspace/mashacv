#ifndef RESULTINFO_H
#define RESULTINFO_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include "image.h"


class ResultInfo : public QWidget
{
    Q_OBJECT
public:
    ResultInfo(std::vector<size_t>*, QString, Image*, QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QPlainTextEdit* edit;
    QPushButton* bclose;
    QPushButton* bsave;
    QGridLayout* layout;
    QString fileName;
private slots:
    void ssave();
    void sclose();
};

#endif // RESULTINFO_H
