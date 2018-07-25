#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QFileDialog>
#include <QThread>
#include <QFile>
#include <QMessageBox>

void Worker::doWork(QStringList parameter)
{
   int count = parameter.count();
   int base = 100 / count;
   int index = 1;

   QStringList::iterator it = parameter.begin();
   for(; it != parameter.end(); ++it)
   {
        emit addJsonFilePath(*it);
        // parse json file
        QFile file(*it);
        if(!file.open(QIODevice::ReadOnly))
        {
             emit resultReady(tr("Read file failed, please check file ==> ").append(*it));
             continue;
        }
        QByteArray data = file.readAll();
        QJsonParseError jsonParErr;
        QJsonDocument jd = QJsonDocument::fromJson(data, &jsonParErr);
        if(jd.isNull() || jsonParErr.error != QJsonParseError::NoError)
        {
            emit resultReady(tr("Json file parsed error, please check file ==> ").append(*it));
        }
        else
        {
            emit resultReady(tr("Parsed OK ==> ").append(*it));
        }

        if(index == count)
        {
            emit updateProgessBar(100);
            emit updateProgessBarText("Completed parse");
        }
        else
        {
            emit updateProgessBar(index * base);
            emit updateProgessBarText("Parsing json...");
        }

        ++index;
   }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);

    ui->label->setText("Please select json file");

    //mWorkerThread = QSharedPointer<QThread>(new QThread);
    //mWorker = QSharedPointer<Worker>(new Worker);
    mWorkerThread = new QThread;
    mWorker = new Worker;
    mWorker->moveToThread(mWorkerThread);
    connect(mWorker, &Worker::resultReady, this, &MainWindow::handleResults);
    connect(mWorker, &Worker::addJsonFilePath, this, &MainWindow::jsonFilePathAdded);
    connect(mWorker, &Worker::updateProgessBar, this, &MainWindow::progressBarUpdated);
    connect(mWorker, &Worker::updateProgessBarText, this, &MainWindow::progressBarTextUpdated);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::jsonFilesSelected);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::allListWgtDataCleared);
    connect(this, &MainWindow::operate, mWorker, &Worker::doWork);

    if(0 != ui->result->count())
    {
        ui->result->clear();
    }

    if(0 != ui->jsonFilePath->count())
    {
        ui->jsonFilePath->clear();
    }

    mWorkerThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    mWorkerThread->quit();
    mWorkerThread->wait();
}

void MainWindow::handleResults(const QString &parameter)
{
    ui->result->addItem(parameter);
}

void MainWindow::jsonFilePathAdded(const QString &parameter)
{
   ui->jsonFilePath->addItem(parameter);
}

void MainWindow::progressBarUpdated(const int parameter)
{
   ui->progressBar->setValue(parameter);
}

void MainWindow::jsonFilesSelected(bool)
{
   QStringList fileList = QFileDialog::getOpenFileNames(this, tr("select json files"), "", "Json files (*.json)");
   emit operate(fileList);
}

void MainWindow::allListWgtDataCleared(bool)
{
   ui->result->clear();
   ui->jsonFilePath->clear();
   ui->label->setText("Completed clear");
}

void MainWindow::progressBarTextUpdated(const QString &parameter)
{
   ui->label->setText(parameter);
}
