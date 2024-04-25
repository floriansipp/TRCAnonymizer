#include "DuplicateCheckWorker.h"
#include <QFileInfo>
#include <QDirIterator>

DuplicateCheckWorker::DuplicateCheckWorker(QString rootDir)
{
    m_rootDir = rootDir;
}

DuplicateCheckWorker::~DuplicateCheckWorker()
{

}

void DuplicateCheckWorker::Process()
{
    emit sendLogInfo(QString::fromStdString("Starting duplicate checking process."));
    emit sendLogInfo("");

    QMultiMap<QString, QString> fileMap;
    QDirIterator it(m_rootDir, QStringList() << "*.trc" << "*.edf", QDir::Files , QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString path = it.next();
        QFileInfo f(path);
        fileMap.insert(f.fileName(), f.absoluteFilePath());
    }

    int duplicateCount = 0;
    QMultiMap<QString, QString>::iterator i;
    QSet<QString> seenKeys;
    for (i = fileMap.begin(); i != fileMap.end(); ++i)
    {
        if (seenKeys.contains(i.key()))
        {
            // We already have this key, so we skip it
            continue;
        }

        QList<QString> values = fileMap.values(i.key());
        if (values.size() > 1)
        {
            duplicateCount++;
            emit sendLogInfo(QString::fromStdString("Files with the same name: ") + i.key());
            foreach (QString value, values)
            {
                emit sendLogInfo(QString::fromStdString("    ") + value);
            }
            emit sendLogInfo("");
        }

        // Add the key to the seenKeys set
        seenKeys.insert(i.key());
    }

    if(duplicateCount == 0)
    {
        emit sendLogInfo("No Duplicates were found");
        emit sendLogInfo("");
    }

    emit sendLogInfo(QString::fromStdString("Duplicate checking process finished."));
    emit sendLogInfo(QString::fromStdString(""));

    emit finished();
}
