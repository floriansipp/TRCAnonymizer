#ifndef ANONYMIZATIONWORKER_H
#define ANONYMIZATIONWORKER_H

#include <QCoreApplication>
#include <QObject>
#include <vector>
#include <QString>
#include "GenericMontage.h"
#include "IFile.h"

class AnonymizationWorker : public QObject
{
    Q_OBJECT

public:
    AnonymizationWorker(std::vector<std::string> files, bool copyAll, IFile* copyFileInfo);
    ~AnonymizationWorker();
    void Process();

private:
    void CopyAndAnonFile(IFile* f);
    IFile* GetFile(std::string path);

signals:
    void finished();
    void sendLogInfo(QString s);
    void sendErrorLogInfo(QString s);
    void progress(double percentage);

private:
    std::vector<std::string> m_files;
    bool m_copyDataAll = false;
    IFile* m_templateFile = nullptr;
    IFile* m_workingFile = nullptr;
    std::string name = "";
    std::string surname = "";
    unsigned char d, rd, rth;
    unsigned char m, rm, rtm;
    unsigned char y, ry, rts;
    std::vector<GenericMontage> m_montages;
};

#endif // ANONYMIZATIONWORKER_H
