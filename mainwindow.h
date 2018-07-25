#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>
#include <QPointer>

namespace Ui {
class MainWindow;
}

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(QStringList parameter);

signals:
    void resultReady(const QString &result);
    void addJsonFilePath(const QString &paremater);
    void updateProgessBar(const int parameter);
    void updateProgessBarText(const QString &parameter);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void handleResults(const QString &parameter);
    void jsonFilePathAdded(const QString &parameter);
    void progressBarUpdated(const int parameter);
    void progressBarTextUpdated(const QString &parameter);
private slots:
    void jsonFilesSelected(bool);
    void allListWgtDataCleared(bool);
signals:
    void operate(QStringList parameter);
private:
    Ui::MainWindow *ui;
    //QSharedPointer<QThread> mWorkerThread;
    //QSharedPointer<Worker> mWorker;
    QPointer<QThread> mWorkerThread;
    QPointer<Worker> mWorker;
};

#endif // MAINWINDOW_H
