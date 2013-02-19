#include <QApplication>
#include "mainwindow.h"
#include <QString>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    //TODO: допилить возможность открытия нескольких файлов
    //TODO: проверить разные charset. И на шindows тоже.
    if (argc == 2)
    {
        w.openImage(QString::fromLocal8Bit(argv[1]));
    }
    else if (argc == 3)
    {
        QString arg1 = QString::fromLocal8Bit(argv[1]);
        QString arg2 = QString::fromLocal8Bit(argv[2]);
        QString filename;
        if (arg1 == "--silent")
            filename = arg2;
        else if (arg2 == "--silent")
        {
            filename = arg1;
        }
        w.openImage(filename);
        qApp->processEvents();
        qApp->processEvents();
        qApp->processEvents();
        w.makeAllThings();
    }
    return a.exec();
}
