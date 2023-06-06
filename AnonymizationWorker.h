#ifndef ANONYMIZATIONWORKER_H
#define ANONYMIZATIONWORKER_H

#include <QCoreApplication>
#include <QObject>
#include <iostream>
#include <vector>
#include <QString>
#include "MicromedFile.h"

class AnonymizationWorker : public QObject
{
    Q_OBJECT

public:
    AnonymizationWorker(std::vector<std::string> files, bool copyAll, MicromedFile* copyFileInfo);
    ~AnonymizationWorker();
    void Process();

private:
    void CopyAndAnonFile(MicromedFile f);

signals:
    void finished();
    void sendLogInfo(QString s);
    void sendErrorLogInfo(QString s);
    void progress(double percentage);

private:
    std::vector<std::string> m_files;
    bool m_copyDataAll = false;
    MicromedFile* m_templateFile = nullptr;
    std::string name = "";
    std::string surname = "";
    unsigned char d, rd, rth;
    unsigned char m, rm, rtm;
    unsigned char y, ry, rts;
    std::vector<montagesOfTrace> m_montages;
};

#endif // ANONYMIZATIONWORKER_H
