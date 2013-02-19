#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "graphicsview.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
    this->isImageOpened = false;
    ui->mainToolBar->hide(); //пока не использую
    //уже использую  ui->statusBar->hide(); //пока не использую
    this->setWindowIcon(QIcon(":/icons/icon.png"));
    //this->setRussianLanguage();
    this->exit = false;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeAllThings()
{
    this->imageProcessor->makeAllThings();
    this->exit = true;
    QTimer::singleShot(0, this, SLOT(loopStarted()));
}

#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <QStringList>
void MainWindow::openImage(QString initFileName)
{
    qDebug() << "opening" << initFileName;
    QCursor cursor = this->cursor();
    this->setCursor(Qt::BusyCursor);
    QString fileName;
    if (initFileName.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(this, "Open Image", "/home/misha/", "Image Files (*.png *.jpg)");
    }
    else
        fileName = initFileName;

    if (fileName != "")
    {
        if (this->isImageOpened)
        {
            //open new instance
            QProcess::startDetached(qApp->applicationFilePath(), QStringList(fileName));
        }
        else
        {
            this->imageProcessor = new ImageProcessor(fileName, this);
            if (this->imageProcessor)
            {
                this->centralWidget()->layout()->addWidget(this->imageProcessor->getMainWidget());
                QList<QDockWidget*> docks = this->imageProcessor->getDockWidgets();
                for (int i = 0; i < docks.count(); i++)
                {
                    if (docks.at(i)->objectName() == "ColorDockWidget")
                        this->addDockWidget(Qt::TopDockWidgetArea, docks.at(i));
                    else
                        this->addDockWidget(Qt::LeftDockWidgetArea, docks.at(i));
                }
                ui->menuView->addMenu(this->createPopupMenu()); //TODO: переделать это
                this->imageProcessor->setObjectName("imageProcessor");
            }
            this->isImageOpened = true;
            QMetaObject::connectSlotsByName(this);
        }
    }
    this->setCursor(cursor);
    qApp->processEvents();
    imageProcessor->loadMySettings();
}

//TODO: переписать это
#include <QTranslator>
void MainWindow::setRussianLanguage()
{
    QTranslator* transl = new QTranslator();
    transl->load(":/languages/ru");
    qApp->installTranslator(transl);
}

void MainWindow::setEnglishLanguage()
{

}

void MainWindow::on_imageProcessor_notifyHoverItemSize(int size)
{
    if (size == 0)
    {
        this->ui->statusBar->clearMessage();
        QToolTip::hideText();
    }
    else
    {
        this->ui->statusBar->showMessage(QString::number(size));
        QPoint cursorPos = QCursor::pos();
        QPoint p = this->mapFromGlobal(cursorPos);
        QToolTip::showText(cursorPos, QString::number(size), this, QRect(p, p));
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
    //TODO: допилить это
}
#include <QUrl>
#include <QMimeData>
void MainWindow::dropEvent(QDropEvent *event)
{
    //TODO: допилить и это
    //TODO: допилить возможность бросать файлы на GraphicsView
    //TODO: подумать, что делать, если файлов несколько.
    this->openImage(event->mimeData()->urls().at(0).toLocalFile());
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->exit();
}

void MainWindow::loopStarted()
{

    if (exit)
    {
        qApp->exit();
    }
}
