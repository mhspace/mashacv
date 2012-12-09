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
    if (argc > 1)
    {
        w.openImage(QString::fromLocal8Bit(argv[1]));
    }
    return a.exec();
}
