#include "zipworker.h"
#include <JlCompress.h>

ZipWorker::ZipWorker(QStringList &songListIn, QString &outputFolderIn)
    : songList(songListIn)
    , outputFolder(outputFolderIn)
{

}

void ZipWorker::run()
{
    QString archivePath;
    for (int i = 0; i < songList.size(); i++)
    {
        archivePath = songList.at(i);
        archivePath.remove(0, archivePath.lastIndexOf('/'));
        archivePath += ".osz";
        archivePath.push_front(outputFolder);

        JlCompress::compressDir(archivePath, songList.at(i));

        emit archiveFinished();
    }
    emit threadFinished();
}
