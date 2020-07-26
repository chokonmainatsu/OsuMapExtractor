#ifndef ZIPWORKER_H
#define ZIPWORKER_H

#include <QRunnable>
#include <QStringList>
#include <JlCompress.h>


class ZipWorker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ZipWorker(QStringList &songListIn, QString &outputFolderIn);

    void run() override;

public: Q_SIGNALS:
    void archiveFinished();
    void threadFinished();

private:
    QStringList songList;
    QString outputFolder;
};

#endif // ZIPWORKER_H
