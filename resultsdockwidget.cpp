#include "resultsdockwidget.h"
#include "ui_resultsdockwidget.h"

ResultsDockWidget::ResultsDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ResultsDockWidget)
{
    ui->setupUi(this);
    this->disableResults();
}

ResultsDockWidget::~ResultsDockWidget()
{
    delete ui;
}

void ResultsDockWidget::disableResults()
{
    ui->dockWidgetContents->setEnabled(false);
}

void ResultsDockWidget::showResults(QString results)
{
    this->results = results;
    if (this->filteredResults.isEmpty())
        ui->plainTextEdit->setPlainText(this->results);
    else
    {
        ui->plainTextEdit->setPlainText(this->filteredResults + "\n" + this->results);
    }
    ui->dockWidgetContents->setEnabled(true);
}

void ResultsDockWidget::showFilteredResults(QString results)
{
    this->filteredResults = results;
    if (this->filteredResults.isEmpty())
        ui->plainTextEdit->setPlainText(this->results);
    else
    {
        ui->plainTextEdit->setPlainText(this->filteredResults + "\n" + this->results);
    }
    ui->dockWidgetContents->setEnabled(true);
}

void ResultsDockWidget::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
