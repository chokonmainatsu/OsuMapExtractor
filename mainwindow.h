#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString osuSongsFolder;
    QString outputFolder;
    QThreadPool* threadPool;
    int totalThreads;
    int threadsFinished;

private Q_SLOTS:
    void onOsuFolderOpen();
    void onOutputFolderOpen();
    void onOsuExtract();
    void increaseProgressBar();
    void threadFinished();

};
#endif // MAINWINDOW_H
