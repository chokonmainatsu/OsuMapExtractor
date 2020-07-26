#include <QThread>
#include <QThreadPool>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QDirIterator>
#include <QVector>
#include <JlCompress.h>
#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "zipworker.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , osuSongsFolder(qgetenv("LOCALAPPDATA") + "\\osu!\\Songs")
    , outputFolder(qgetenv("USERPROFILE") + "\\Desktop")
    , threadPool(new QThreadPool)
{
    ui->setupUi(this);

    // ui setup

    ui->lineEditOsuFolder->setPlaceholderText(osuSongsFolder);

    ui->lineEditOutput->setPlaceholderText(outputFolder);

    int idealThreadCount = QThread::idealThreadCount();
    ui->spinBoxThreads->setMaximum(idealThreadCount);
    ui->spinBoxThreads->setValue(idealThreadCount);

    ui->progressBar->setVisible(false);

    // event setup

    connect(ui->pushButtonOsuFolder, &QPushButton::clicked, this, &MainWindow::onOsuFolderOpen);
    connect(ui->pushButtonOutput, &QPushButton::clicked, this, &MainWindow::onOutputFolderOpen);
    connect(ui->pushButtonExtract, &QPushButton::clicked, this, &MainWindow::onOsuExtract);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOsuFolderOpen()
{
    QFileDialog fd;
    QString path = fd.getExistingDirectory(nullptr, "Select the osu! songs folder", osuSongsFolder);
    path.replace('/', '\\');

    if (!QFile::exists(path + "\\..\\osu!.exe"))
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Warning);
        mb.setText("Warning\n\nThe selected folder is not the songs folder within osu!");
        mb.exec();
    }

    ui->lineEditOsuFolder->setText(path);
}

void MainWindow::onOutputFolderOpen()
{
    QFileDialog fd;
    QString path = fd.getExistingDirectory(nullptr, "Select output folder", outputFolder);
    path.replace('/', '\\');

    QDir dir(path);
    if (!dir.isEmpty())
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Warning);
        mb.setText("Warning\n\nThe selected output folder is not empty!");
        mb.exec();
    }

    ui->lineEditOutput->setText(path);
}

void MainWindow::onOsuExtract()
{
    if (ui->lineEditOsuFolder->text() == "" ||
            ui->lineEditOutput->text() == "")
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setText("Warning\n\nAt least one path is not set!");
        mb.exec();
        return;
    }
    QDir checkDir;
    if (!checkDir.exists(ui->lineEditOsuFolder->text())
            || !checkDir.exists(ui->lineEditOutput->text()))
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setText("Warning\n\nAt least one paths does not exist!");
        mb.exec();
        return;
    }

    ui->pushButtonOsuFolder->setEnabled(false);
    ui->pushButtonOutput->setEnabled(false);
    ui->pushButtonExtract->setEnabled(false);
    ui->spinBoxThreads->setEnabled(false);

    QDirIterator di(ui->lineEditOsuFolder->text());
    QStringList songList;

    di.next();
    di.next();

    while (di.hasNext())
        songList << di.next();

    ui->progressBar->setMaximum(songList.size());
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);

    QString output = ui->lineEditOutput->text();
    output.replace('\\', '/');

    // make threads

    const int maxThreads = qMin(ui->spinBoxThreads->value(), songList.size());

    double tmp = static_cast<double>(songList.size()) / maxThreads;
    if (tmp - (songList.size() / maxThreads) > 0)
        tmp += 1;
    const int archivesPerThread = tmp;

    int songListIterator = 0;

    totalThreads = maxThreads;
    threadsFinished = 0;
    QVector<ZipWorker*> workerList;
    for (int i = 0; i < maxThreads; i++)
    {
        QStringList tmpSongList;
        QString archivePath;

        for (int threadIterator = 0; threadIterator < archivesPerThread && songListIterator < songList.size(); threadIterator++, songListIterator++)
            tmpSongList << songList.at(songListIterator);

        workerList << new ZipWorker(tmpSongList, output);
    }

    for (int i = 0; i < workerList.size(); i++)
    {
        connect(workerList.at(i), &ZipWorker::archiveFinished, this, &MainWindow::increaseProgressBar);
        connect(workerList.at(i), &ZipWorker::threadFinished, this, &MainWindow::threadFinished);
        threadPool->start(workerList.at(i));
    }

    return;
}

void MainWindow::increaseProgressBar()
{
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void MainWindow::threadFinished()
{
    if (++threadsFinished < totalThreads)
        return;

    ui->pushButtonOsuFolder->setEnabled(true);
    ui->pushButtonOutput->setEnabled(true);
    ui->pushButtonExtract->setEnabled(true);
    ui->spinBoxThreads->setEnabled(true);
}

