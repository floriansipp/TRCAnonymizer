#ifndef INFORMATIONEXTRACTIONWORKER_H
#define INFORMATIONEXTRACTIONWORKER_H

#include <QCoreApplication>
#include <QObject>
#include <string>
#include <vector>
#include <QString>
#include "IFile.h"

class InformationExtractionWorker : public QObject
{
    Q_OBJECT

public:
    InformationExtractionWorker(QString csvPath, std::vector<std::string> files);
    ~InformationExtractionWorker();
    void Process();

private:
    IFile* GetFile(std::string path);

signals:
    void finished();
    void sendLogInfo(QString s);
    void sendErrorLogInfo(QString s);
    void progress(double percentage);

private:
    QString m_csvPath = "";
    std::vector<std::string> m_files;
};

#endif // INFORMATIONEXTRACTIONWORKER_H
