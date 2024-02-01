#ifndef TOOLSWORKER_H
#define TOOLSWORKER_H

#include <QCoreApplication>
#include <QObject>
#include <string>
#include <vector>
#include <QString>
#include "IFile.h"

class ToolsWorker : public QObject
{
    Q_OBJECT

public:
    ToolsWorker(QString csvPath, std::vector<std::string> files);
    ~ToolsWorker();
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

#endif // TOOLSWORKER_H
