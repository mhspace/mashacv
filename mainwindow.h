#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QToolTip>

#include "imageprocessor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void openImage(QString initFileName = "");
    void setRussianLanguage();
    void setEnglishLanguage();
    void on_imageProcessor_notifyHoverItemSize(int size);

protected:
    void changeEvent(QEvent *e);
    void dragEnterEvent(QDragEnterEvent * event);
    void dropEvent( QDropEvent * event );
    
private slots:
    void on_actionQuit_triggered();

private:
    Ui::MainWindow *ui;
    ImageProcessor *imageProcessor;
    bool isImageOpened;
};

#endif // MAINWINDOW_H
