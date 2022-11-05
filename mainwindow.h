#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QToolTip>
#include <QTranslator>

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
    void makeAllThings();
    
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
    void loopStarted();
    void on_actionOpen_triggered();
    void on_actionRussian_triggered();
    void on_actionEnglish_triggered();

private:
    Ui::MainWindow *ui;
    ImageProcessor *imageProcessor;
    QTranslator *ru;
    bool isImageOpened;
    bool exit;
};

#endif // MAINWINDOW_H
