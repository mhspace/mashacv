#include "resultinfo.h"

ResultInfo::ResultInfo(std::vector<size_t>* edges, QString fileName, Image* image, QWidget *parent) :
    QWidget(parent)
{
    this->layout = new QGridLayout();
    this->bsave = new QPushButton("Save");
    this->connect(this->bsave, SIGNAL(clicked()), this, SLOT(ssave()));
    this->bclose = new QPushButton("Close");
    this->connect(this->bclose, SIGNAL(clicked()), this, SLOT(sclose()));
    this->edit = new QPlainTextEdit();

    this->layout->addWidget(edit, 0, 0, 1, 3);
    this->layout->addWidget(this->bsave, 1, 0);
    this->layout->addWidget(this->bclose, 1, 2);

    this->setLayout(this->layout);

    this->fileName = fileName;

    this->edit->appendPlainText("Number: " + QString::number(edges->size()));
    size_t summ = 0;
    size_t summsq = 0;
    for (int i = 0; i < edges->size(); i++)
    {
        summ += edges->at(i);
        summsq += (edges->at(i)*edges->at(i));
    }

    float d = (summsq/edges->size()) - ((summ/edges->size())*(summ/edges->size()));
    this->edit->appendPlainText("Average: " + QString::number(summ/edges->size()));
    Mat* merged = image->getRasterMergedMask();

    this->edit->appendPlainText("Summ: " + QString::number(summ));

    size_t area = 0;

    for (int y = 0; y < image->selMask.rows; y++)
        for (int x = 0; x < image->selMask.cols; x++)
            if (image->selMask.at<uchar>(y, x) == (uchar)1)
                area++;

    this->edit->appendPlainText("Area size: " + QString::number(area));
    this->edit->appendPlainText("Occupied area: " + QString::number((100*summ)/area) + "%");
    this->edit->appendPlainText("D: " + QString::number(d));

    this->show();
}

void ResultInfo::ssave()
{
    QString name = this->fileName;
    name.append(".");
    name.append(QDateTime::currentDateTime().toString("yyyy.mm.dd hh.mm.ss"));
    name.append(".txt");
    QFile* file = new QFile(name);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        this->bsave->setText("Save error");
        return;
    }
    else
    {
        QTextStream out(file);
        out << this->edit->toPlainText();
        out.flush();
        file->flush();
        file->close();

        this->bsave->setText("Saved");
    }
}

void ResultInfo::sclose()
{
    this->edit->~QPlainTextEdit();
    this->bsave->~QPushButton();
    this->bclose->~QPushButton();
    this->layout->~QGridLayout();
    this->close();
}
